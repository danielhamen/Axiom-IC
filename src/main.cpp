#include "core/operations.hpp"
#include "core/optimizer.hpp"
#include "core/verifier.hpp"
#include "parser/parser.hpp"
#include "parser/tokenizer.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

enum class JsonMode {
    None,
    Tokens,
    Parse,
    Validate,
    Dump
};

struct CliOptions {
    std::optional<uint32_t> seed;
    bool show_help = false;
    bool time = false;
    bool benchmark = false;
    size_t warmup = 0;
    size_t runs = 10;

    bool profile = false;
    bool opcode_stats = false;
    bool trace_time = false;

    bool dump_tokens = false;
    bool dump_bytecode = false;
    bool dump_constants = false;
    bool dump_functions = false;
    bool dump_symbols = false;
    bool dump_labels = false;
    JsonMode json_mode = JsonMode::None;

    std::vector<std::string> input_files;
};

std::string format_duration(std::chrono::nanoseconds ns) {
    using namespace std::chrono;
    std::ostringstream out;
    out << std::fixed << std::setprecision(3);
    if (ns >= milliseconds(1)) {
        out << duration<double, std::milli>(ns).count() << " ms";
    } else if (ns >= microseconds(1)) {
        out << duration<double, std::micro>(ns).count() << " us";
    } else {
        out << duration<double, std::nano>(ns).count() << " ns";
    }

    return out.str();
}

[[noreturn]] void throw_cli_error(const std::string& message) {
    throw std::runtime_error("CLI error: " + message);
}

size_t parse_size(std::string_view value, const std::string& name) {
    if (value.empty()) {
        throw_cli_error("Missing value for --" + name);
    }

    size_t parsed = 0;
    try {
        parsed = std::stoull(std::string(value));
    } catch (...) {
        throw_cli_error("Invalid numeric value for --" + name + ": '" + std::string(value) + "'");
    }

    return parsed;
}

uint32_t parse_seed(std::string_view value) {
    if (value.empty()) {
        throw_cli_error("Missing value for --seed");
    }

    unsigned long long parsed = 0;
    try {
        parsed = std::stoull(std::string(value));
    } catch (...) {
        throw_cli_error("Invalid seed value: '" + std::string(value) + "'");
    }

    if (parsed > std::numeric_limits<uint32_t>::max()) {
        throw_cli_error("Seed is out of range for 32-bit value");
    }

    return static_cast<uint32_t>(parsed);
}

JsonMode parse_json_mode(std::string_view value) {
    if (value.empty() || value == "dump" || value == "load") {
        return JsonMode::Dump;
    }
    if (value == "tokens") {
        return JsonMode::Tokens;
    }
    if (value == "parse") {
        return JsonMode::Parse;
    }
    if (value == "validate") {
        return JsonMode::Validate;
    }
    throw_cli_error("Invalid --json mode: '" + std::string(value) + "' (expected tokens, parse, validate, dump, or load)");
}

CliOptions parse_cli(int argc, char** argv) {
    CliOptions options;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (!arg.starts_with("--")) {
            options.input_files.push_back(arg);
            continue;
        }

        if (arg == "--help") {
            options.show_help = true;
            continue;
        }

        if (arg == "--time") {
            options.time = true;
            continue;
        }

        if (arg == "--benchmark") {
            options.benchmark = true;
            continue;
        }

        if (arg == "--profile") {
            options.profile = true;
            continue;
        }

        if (arg == "--opcode-stats") {
            options.opcode_stats = true;
            continue;
        }

        if (arg == "--trace-time") {
            options.trace_time = true;
            continue;
        }

        if (arg == "--tokens") {
            options.dump_tokens = true;
            continue;
        }

        if (arg == "--bytecode") {
            options.dump_bytecode = true;
            continue;
        }

        if (arg == "--constants") {
            options.dump_constants = true;
            continue;
        }

        if (arg == "--functions") {
            options.dump_functions = true;
            continue;
        }

        if (arg == "--symbols") {
            options.dump_symbols = true;
            continue;
        }

        if (arg == "--labels") {
            options.dump_labels = true;
            continue;
        }

        if (arg == "--json") {
            options.json_mode = JsonMode::Dump;
            continue;
        }

        if (arg.starts_with("--json=")) {
            options.json_mode = parse_json_mode(arg.substr(std::string("--json=").size()));
            continue;
        }

        if (arg.starts_with("--seed=")) {
            options.seed = parse_seed(arg.substr(std::string("--seed=").size()));
            continue;
        }

        if (arg.starts_with("--warmup=")) {
            options.warmup = parse_size(arg.substr(std::string("--warmup=").size()), "warmup");
            continue;
        }

        if (arg.starts_with("--runs=")) {
            options.runs = parse_size(arg.substr(std::string("--runs=").size()), "runs");
            continue;
        }

        throw_cli_error("Unknown flag: " + arg);
    }

    if (options.input_files.empty() && !options.show_help) {
        options.input_files.push_back("Main.aic");
    }

    if (options.runs == 0) {
        throw_cli_error("--runs must be greater than 0");
    }

    return options;
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(aic::format_error_context(aic::ErrorPhase::Parse,
                                                           "Could not open input file: " + path));
    }

    std::string in;
    std::string line;
    while (std::getline(file, line)) {
        in += line + "\n";
    }

    return in;
}

namespace fs = std::filesystem;

struct ModuleExportInfo {
    fs::path path;
    std::string module_name;
    std::unordered_map<std::string, std::string> exported_functions;
    std::unordered_map<std::string, aic::ConstantPoolRange> exported_constant_pools;
};

struct ModuleLinkContext {
    std::unordered_map<std::string, ModuleExportInfo> linked_modules;
    std::unordered_set<std::string> active_modules;
};

std::string canonical_key(const fs::path& path) {
    return fs::weakly_canonical(path).string();
}

std::string sanitize_module_id(const aic::ModuleImport& import) {
    std::string out;
    for (const std::string& part : import.path) {
        if (!out.empty()) {
            out += "_";
        }
        out += part;
    }
    return out;
}

fs::path resolve_module_file(const fs::path& importer_dir, const aic::ModuleImport& import) {
    fs::path path = importer_dir;
    for (const std::string& segment : import.path) {
        path /= segment;
    }
    path += ".aic";
    return path;
}

aic::Program parse_file_to_program(const fs::path& path) {
    std::string in = read_file(path.string());
    std::vector<aic::Token> tokens = aic::tokenize(in);
    return aic::parse(tokens);
}

void validate_module_file(const aic::Program& program, const fs::path& path, bool required_module) {
    if (program.module_name.empty()) {
        if (required_module) {
            throw std::runtime_error(aic::format_error_context(
                aic::ErrorPhase::Parse,
                "Cannot import '" + path.string() + "' because it has no `.module <moduleName>` declaration"));
        }
        return;
    }

    const std::string stem = path.stem().string();
    if (program.module_name != stem) {
        throw std::runtime_error(aic::format_error_context(
            aic::ErrorPhase::Parse,
            "Module declaration `.module " + program.module_name +
                "` must match filename `" + stem + ".aic`"));
    }
}

bool is_link_function_reference(aic::OperationKind op, size_t operand_index) {
    switch (op) {
        case aic::OperationKind::CALL:
            return operand_index == 0;
        case aic::OperationKind::LIST_MAP:
        case aic::OperationKind::LIST_FILTER:
        case aic::OperationKind::LIST_REDUCE:
        case aic::OperationKind::STRUCT_DEF_METHOD:
            return operand_index == 2;
        case aic::OperationKind::STRUCT_DEF_VALIDATOR:
            return operand_index == 1;
        default:
            return false;
    }
}

void refresh_instruction_short_operands(aic::Instruction& ins) {
    aic::Operand null_op{aic::OperandKind::None};
    ins.x = ins.operands.size() > 0 ? ins.operands[0] : null_op;
    ins.y = ins.operands.size() > 1 ? ins.operands[1] : null_op;
    ins.z = ins.operands.size() > 2 ? ins.operands[2] : null_op;
}

void rewrite_instruction_for_link(aic::Instruction& ins,
                                  size_t constant_offset,
                                  const std::unordered_map<std::string, std::string>& local_functions,
                                  const std::unordered_map<std::string, std::string>& imported_functions) {
    for (size_t i = 0; i < ins.operands.size(); i++) {
        aic::Operand& operand = ins.operands[i];
        if (operand.kind == aic::OperandKind::Constant) {
            operand.value += static_cast<int64_t>(constant_offset);
        }

        if (!is_link_function_reference(ins.op, i)) {
            continue;
        }

        if (operand.kind != aic::OperandKind::Function && operand.kind != aic::OperandKind::Label) {
            continue;
        }

        auto local_it = local_functions.find(operand.strval);
        if (local_it != local_functions.end()) {
            operand.strval = local_it->second;
            operand.kind = aic::OperandKind::Function;
            continue;
        }

        auto import_it = imported_functions.find(operand.strval);
        if (import_it != imported_functions.end()) {
            operand.strval = import_it->second;
            operand.kind = aic::OperandKind::Function;
        }
    }

    refresh_instruction_short_operands(ins);
}

void add_imported_symbol(std::unordered_map<std::string, std::string>& function_aliases,
                         aic::Program& linked,
                         const std::string& symbol,
                         const ModuleExportInfo& exports) {
    auto fn_it = exports.exported_functions.find(symbol);
    if (fn_it != exports.exported_functions.end()) {
        if (function_aliases.contains(symbol)) {
            throw std::runtime_error(aic::format_error_context(aic::ErrorPhase::Parse,
                                                               "Ambiguous imported function: " + symbol));
        }
        function_aliases[symbol] = fn_it->second;
        return;
    }

    auto pool_it = exports.exported_constant_pools.find(symbol);
    if (pool_it != exports.exported_constant_pools.end()) {
        if (linked.constant_pools.contains(symbol)) {
            throw std::runtime_error(aic::format_error_context(aic::ErrorPhase::Parse,
                                                               "Duplicate imported constant pool: " + symbol));
        }
        linked.constant_pools[symbol] = pool_it->second;
        return;
    }

    throw std::runtime_error(aic::format_error_context(
        aic::ErrorPhase::Parse,
        "Module `" + exports.module_name + "` does not export symbol: " + symbol));
}

ModuleExportInfo link_module_file(const fs::path& module_path,
                                  const aic::ModuleImport& import,
                                  aic::Program& linked,
                                  ModuleLinkContext& context);

std::unordered_map<std::string, std::string> collect_import_aliases(const fs::path& importer_dir,
                                                                    const std::vector<aic::ModuleImport>& imports,
                                                                    aic::Program& linked,
                                                                    ModuleLinkContext& context) {
    std::unordered_map<std::string, std::string> function_aliases;

    for (const aic::ModuleImport& import : imports) {
        ModuleExportInfo exports = link_module_file(resolve_module_file(importer_dir, import),
                                                    import,
                                                    linked,
                                                    context);

        if (import.imports_all()) {
            for (const auto& [symbol, _] : exports.exported_functions) {
                add_imported_symbol(function_aliases, linked, symbol, exports);
            }
            for (const auto& [symbol, _] : exports.exported_constant_pools) {
                add_imported_symbol(function_aliases, linked, symbol, exports);
            }
            continue;
        }

        for (const std::string& symbol : import.selected_symbols) {
            add_imported_symbol(function_aliases, linked, symbol, exports);
        }
    }

    return function_aliases;
}

ModuleExportInfo link_module_file(const fs::path& module_path,
                                  const aic::ModuleImport& import,
                                  aic::Program& linked,
                                  ModuleLinkContext& context) {
    const fs::path canonical_path = fs::weakly_canonical(module_path);
    const std::string key = canonical_path.string();

    auto cached = context.linked_modules.find(key);
    if (cached != context.linked_modules.end()) {
        return cached->second;
    }

    if (!fs::exists(canonical_path)) {
        throw std::runtime_error(aic::format_error_context(aic::ErrorPhase::Parse,
                                                           "Imported module file does not exist: " +
                                                               module_path.string()));
    }
    if (!context.active_modules.insert(key).second) {
        throw std::runtime_error(aic::format_error_context(aic::ErrorPhase::Parse,
                                                           "Module import cycle detected at: " +
                                                               module_path.string()));
    }

    aic::Program module = parse_file_to_program(canonical_path);
    validate_module_file(module, canonical_path, true);
    linked.imported_categories.insert(module.imported_categories.begin(), module.imported_categories.end());

    if (!import.path.empty() && module.module_name != import.path.back()) {
        throw std::runtime_error(aic::format_error_context(
            aic::ErrorPhase::Parse,
            "Import path `" + import.module_path() + "` resolved to module `" + module.module_name +
                "`, expected `" + import.path.back() + "`"));
    }

    std::unordered_map<std::string, std::string> imported_functions =
        collect_import_aliases(canonical_path.parent_path(), module.module_imports, linked, context);

    std::unordered_map<std::string, std::string> local_functions;
    const std::string module_id = sanitize_module_id(import);
    for (size_t i = 0; i < module.functions.size(); i++) {
        const aic::Function* fn = module.functions.at(i);
        local_functions[fn->name] = "@module/" + module_id + "/" + fn->name;
    }

    for (const auto& [symbol, _] : imported_functions) {
        if (local_functions.contains(symbol)) {
            throw std::runtime_error(aic::format_error_context(
                aic::ErrorPhase::Parse,
                "Imported symbol conflicts with local function in module `" + module.module_name + "`: " + symbol));
        }
    }

    const size_t constant_offset = linked.constants.size();
    linked.constants.insert(linked.constants.end(), module.constants.begin(), module.constants.end());

    for (size_t i = 0; i < module.functions.size(); i++) {
        aic::Function fn = *module.functions.at(i);
        fn.name = local_functions.at(fn.name);
        for (aic::Instruction& ins : fn.ins) {
            rewrite_instruction_for_link(ins, constant_offset, local_functions, imported_functions);
        }
        linked.functions.insert(std::move(fn));
    }

    ModuleExportInfo exports;
    exports.path = canonical_path;
    exports.module_name = module.module_name;

    for (const std::string& symbol : module.exported_symbols) {
        auto fn_it = local_functions.find(symbol);
        if (fn_it != local_functions.end()) {
            exports.exported_functions[symbol] = fn_it->second;
            continue;
        }

        auto pool_it = module.constant_pools.find(symbol);
        if (pool_it != module.constant_pools.end()) {
            aic::ConstantPoolRange pool = pool_it->second;
            pool.start += constant_offset;
            exports.exported_constant_pools[symbol] = pool;
            continue;
        }

        throw std::runtime_error(aic::format_error_context(
            aic::ErrorPhase::Parse,
            "Module `" + module.module_name + "` exports unknown symbol: " + symbol));
    }

    context.active_modules.erase(key);
    context.linked_modules.emplace(key, exports);
    return exports;
}

aic::Program load_program_with_modules(const std::string& filename) {
    const fs::path root_path = fs::weakly_canonical(fs::path(filename));
    aic::Program root = parse_file_to_program(root_path);
    validate_module_file(root, root_path, false);

    aic::Program linked;
    linked.pc = 0;
    linked.fc = 0;
    linked.imported_categories = root.imported_categories;
    linked.module_name = root.module_name;
    linked.constants = root.constants;
    linked.constant_pools = root.constant_pools;

    ModuleLinkContext context;
    std::unordered_map<std::string, std::string> imported_functions =
        collect_import_aliases(root_path.parent_path(), root.module_imports, linked, context);

    std::unordered_map<std::string, std::string> local_functions;
    for (size_t i = 0; i < root.functions.size(); i++) {
        const aic::Function* fn = root.functions.at(i);
        if (imported_functions.contains(fn->name)) {
            throw std::runtime_error(aic::format_error_context(
                aic::ErrorPhase::Parse,
                "Imported function conflicts with local function: " + fn->name));
        }
        local_functions[fn->name] = fn->name;
    }

    for (size_t i = 0; i < root.functions.size(); i++) {
        aic::Function fn = *root.functions.at(i);
        for (aic::Instruction& ins : fn.ins) {
            rewrite_instruction_for_link(ins, 0, local_functions, imported_functions);
        }
        linked.functions.insert(std::move(fn));
    }

    linked.exported_symbols = root.exported_symbols;
    return linked;
}

void print_help() {
    std::cout << "AxiomIC compiler/runtime (aic)\n"
              << "Usage: aic [flags] [input_files...]\n\n"
              << "If no input file is provided, defaults to Main.aic.\n\n"
              << "Execution flags:\n"
              << "  --seed=N        Set deterministic random seed (e.g. --seed=42)\n"
              << "  --time          Print one execution wall-clock time\n"
              << "  --benchmark     Run repeated benchmark iterations and print stats\n"
              << "  --warmup=N      Number of warmup runs before benchmark\n"
              << "  --runs=N        Number of benchmark runs (default: 10)\n"
              << "  --profile       Print function/opcode timing breakdown\n"
              << "  --opcode-stats  Print opcode execution counts\n"
              << "  --trace-time    Print per-instruction timing\n\n"
              << "Introspection flags:\n"
              << "  --tokens        Print token stream\n"
              << "  --bytecode      Print parsed bytecode/instructions\n"
              << "  --constants     Print constants table\n"
              << "  --functions     Print function list\n"
              << "  --symbols       Print discovered symbol references\n"
              << "  --labels        Print labels per function\n"
              << "  --json[=MODE]   Emit JSON and do not execute. MODE: tokens, parse, validate, dump, load\n"
              << "  --help          Show this help menu\n";
}

void print_bytecode(const aic::Program& vm) {
    for (size_t i = 0; i < vm.functions.size(); i++) {
        const aic::Function* fn = vm.functions.at(i);
        std::cout << "Function " << fn->name << " (index=" << i << ")" << std::endl;
        for (size_t pc = 0; pc < fn->ins.size(); pc++) {
            std::cout << "  [" << pc << "] " << fn->ins[pc].to_string() << std::endl;
        }
    }
}

void print_constants(const aic::Program& vm) {
    for (size_t i = 0; i < vm.constants.size(); i++) {
        const aic::Value& v = vm.constants.at(i);
        std::cout << "@" << i << " kind=" << aic::value_kind_to_string(v.kind)
                  << " value=" << v.to_str() << std::endl;
    }
    for (const auto& [name, pool] : vm.constant_pools) {
        std::cout << ".const " << name
                  << " start=@" << pool.start
                  << " count=" << pool.count
                  << std::endl;
    }
}

void print_functions(const aic::Program& vm) {
    for (size_t i = 0; i < vm.functions.size(); i++) {
        const aic::Function* fn = vm.functions.at(i);
        std::cout << "[" << i << "] " << fn->name << " (ins=" << fn->ins.size() << ")" << std::endl;
    }
}

void print_labels(const aic::Program& vm) {
    for (size_t i = 0; i < vm.functions.size(); i++) {
        const aic::Function* fn = vm.functions.at(i);
        std::cout << "Function " << fn->name << " labels:" << std::endl;
        for (const auto& [label, pc] : fn->labels) {
            std::cout << "  " << label << " -> " << pc << std::endl;
        }
    }
}

void print_symbols(const aic::Program& vm) {
    std::unordered_map<std::string, size_t> counts;

    for (size_t i = 0; i < vm.functions.size(); i++) {
        const aic::Function* fn = vm.functions.at(i);
        counts[fn->name]++;

        for (const aic::Instruction& ins : fn->ins) {
            for (const aic::Operand& op : ins.operands) {
                if (op.kind == aic::OperandKind::Function || op.kind == aic::OperandKind::Label) {
                    counts[op.strval]++;
                }
            }
        }
    }

    for (const auto& [symbol, count] : counts) {
        std::cout << symbol << " (refs=" << count << ")" << std::endl;
    }
}

std::string json_escape(const std::string& in) {
    std::string out;
    out.reserve(in.size() + 2);
    for (char ch : in) {
        switch (ch) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    std::ostringstream escaped;
                    escaped << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                            << static_cast<int>(static_cast<unsigned char>(ch));
                    out += escaped.str();
                } else {
                    out += ch;
                }
        }
    }
    return out;
}

std::string json_string(const std::string& in) {
    return "\"" + json_escape(in) + "\"";
}

std::string string_flavor_to_json(aic::StringFlavor flavor) {
    switch (flavor) {
        case aic::StringFlavor::Normal: return "normal";
        case aic::StringFlavor::SQL: return "sql";
        case aic::StringFlavor::Regex: return "regex";
        case aic::StringFlavor::Format: return "format";
    }
    return "normal";
}

std::string token_string_kind_to_json(aic::StringTokenKind kind) {
    switch (kind) {
        case aic::StringTokenKind::Normal: return "normal";
        case aic::StringTokenKind::SQL: return "sql";
        case aic::StringTokenKind::Regex: return "regex";
        case aic::StringTokenKind::Format: return "format";
    }
    return "normal";
}

void print_json_value(std::ostream& out, const aic::Value& value) {
    out << "{";
    out << "\"kind\":" << json_string(aic::value_kind_to_string(value.kind));
    switch (value.kind) {
        case aic::ValueKind::Integer:
            out << ",\"value\":" << value.i;
            break;
        case aic::ValueKind::Float:
            out << ",\"value\":" << value.f;
            break;
        case aic::ValueKind::String:
            out << ",\"value\":" << json_string(value.s)
                << ",\"flavor\":" << json_string(string_flavor_to_json(value.string_flavor));
            break;
        case aic::ValueKind::Boolean:
            out << ",\"value\":" << (value.b ? "true" : "false");
            break;
        case aic::ValueKind::Null:
            out << ",\"value\":null";
            break;
        default:
            out << ",\"display\":" << json_string(value.to_str());
            break;
    }
    out << "}";
}

void print_json_operand(std::ostream& out, const aic::Operand& operand) {
    out << "{";
    out << "\"kind\":" << json_string(operand.kindstr());
    out << ",\"value\":" << operand.value;
    if (!operand.strval.empty()) {
        out << ",\"text\":" << json_string(operand.strval);
    }
    if (operand.resolved) {
        out << ",\"resolved\":true";
    }
    if (operand.kind == aic::OperandKind::Immediate && operand.has_immediate) {
        out << ",\"immediate\":";
        print_json_value(out, operand.immediate);
    }
    out << "}";
}

void print_json_diagnostics(std::ostream& out, const std::vector<aic::VerificationDiagnostic>& diagnostics) {
    out << "[";
    for (size_t i = 0; i < diagnostics.size(); i++) {
        if (i > 0) out << ",";
        const auto& diagnostic = diagnostics[i];
        out << "{";
        out << "\"severity\":" << json_string(diagnostic.severity == aic::DiagnosticSeverity::Error ? "error" : "warning");
        out << ",\"message\":" << json_string(diagnostic.message);
        out << ",\"function\":" << json_string(diagnostic.function_name);
        out << ",\"opcode\":" << json_string(diagnostic.opcode);
        if (diagnostic.pc != static_cast<size_t>(-1)) {
            out << ",\"pc\":" << diagnostic.pc;
        } else {
            out << ",\"pc\":null";
        }
        out << "}";
    }
    out << "]";
}

void print_json_tokens(std::ostream& out, const std::vector<aic::Token>& tokens) {
    out << "[";
    for (size_t i = 0; i < tokens.size(); i++) {
        if (i > 0) out << ",";
        const aic::Token& token = tokens[i];
        out << "{";
        out << "\"type\":" << json_string(aic::token_type_to_string(token.type));
        out << ",\"lexeme\":" << json_string(token.lexeme);
        out << ",\"line\":" << token.line;
        out << ",\"column\":" << token.column;
        if (token.type == aic::TokenType::String) {
            out << ",\"stringFlavor\":" << json_string(token_string_kind_to_json(token.string_kind));
        }
        out << "}";
    }
    out << "]";
}

void print_json_program(std::ostream& out, const aic::Program& vm) {
    out << "{";
    out << "\"module\":" << json_string(vm.module_name);
    out << ",\"imports\":[";
    for (size_t i = 0; i < vm.module_imports.size(); i++) {
        if (i > 0) out << ",";
        out << json_string(vm.module_imports[i].module_path());
    }
    out << "]";
    out << ",\"importedCategories\":[";
    size_t category_index = 0;
    for (const auto& category : vm.imported_categories) {
        if (category_index++ > 0) out << ",";
        out << json_string(category);
    }
    out << "]";
    out << ",\"constants\":[";
    for (size_t i = 0; i < vm.constants.size(); i++) {
        if (i > 0) out << ",";
        out << "{\"index\":" << i << ",\"value\":";
        print_json_value(out, vm.constants[i]);
        out << "}";
    }
    out << "]";
    out << ",\"constantPools\":[";
    size_t pool_index = 0;
    for (const auto& [name, pool] : vm.constant_pools) {
        if (pool_index++ > 0) out << ",";
        out << "{\"name\":" << json_string(name)
            << ",\"start\":" << pool.start
            << ",\"count\":" << pool.count
            << "}";
    }
    out << "]";
    out << ",\"functions\":[";
    for (size_t i = 0; i < vm.functions.size(); i++) {
        if (i > 0) out << ",";
        const aic::Function* fn = vm.functions.at(i);
        out << "{\"index\":" << i
            << ",\"name\":" << json_string(fn->name)
            << ",\"argCount\":" << fn->arg_count;
        out << ",\"labels\":{";
        size_t label_index = 0;
        for (const auto& [label, pc] : fn->labels) {
            if (label_index++ > 0) out << ",";
            out << json_string(label) << ":" << pc;
        }
        out << "},\"instructions\":[";
        for (size_t pc = 0; pc < fn->ins.size(); pc++) {
            if (pc > 0) out << ",";
            const aic::Instruction& ins = fn->ins[pc];
            out << "{\"pc\":" << pc
                << ",\"op\":" << json_string(aic::operation_kind_to_string(ins.op))
                << ",\"operands\":[";
            for (size_t operand_index = 0; operand_index < ins.operands.size(); operand_index++) {
                if (operand_index > 0) out << ",";
                print_json_operand(out, ins.operands[operand_index]);
            }
            out << "]}";
        }
        out << "]}";
    }
    out << "]";
    out << "}";
}

void print_json_file(std::ostream& out,
                     const std::string& filename,
                     JsonMode mode,
                     const std::vector<aic::Token>& tokens,
                     const aic::Program* vm,
                     const std::vector<aic::VerificationDiagnostic>& diagnostics) {
    out << "{";
    out << "\"file\":" << json_string(filename);
    out << ",\"valid\":" << (aic::has_verification_errors(diagnostics) ? "false" : "true");
    out << ",\"diagnostics\":";
    print_json_diagnostics(out, diagnostics);
    if (mode == JsonMode::Tokens || mode == JsonMode::Parse || mode == JsonMode::Dump) {
        out << ",\"tokens\":";
        print_json_tokens(out, tokens);
    }
    if (vm != nullptr && (mode == JsonMode::Parse || mode == JsonMode::Dump)) {
        out << ",\"program\":";
        print_json_program(out, *vm);
    }
    out << "}";
}

void print_benchmark_stats(const std::vector<std::chrono::nanoseconds>& samples) {
    if (samples.empty()) {
        return;
    }

    std::vector<long double> values;
    values.reserve(samples.size());
    for (auto ns : samples) {
        values.push_back(static_cast<long double>(ns.count()));
    }

    const auto [min_it, max_it] = std::minmax_element(values.begin(), values.end());
    const long double total = std::accumulate(values.begin(), values.end(), static_cast<long double>(0));
    const long double mean = total / static_cast<long double>(values.size());

    long double variance = 0;
    for (long double v : values) {
        const long double diff = v - mean;
        variance += diff * diff;
    }
    variance /= static_cast<long double>(values.size());
    const long double stddev = std::sqrt(variance);

    std::cout << "Benchmark stats (runs=" << samples.size() << ")\n"
              << "  avg: " << format_duration(std::chrono::nanoseconds(static_cast<int64_t>(mean))) << "\n"
              << "  min: " << format_duration(std::chrono::nanoseconds(static_cast<int64_t>(*min_it))) << "\n"
              << "  max: " << format_duration(std::chrono::nanoseconds(static_cast<int64_t>(*max_it))) << "\n"
              << "  std: " << format_duration(std::chrono::nanoseconds(static_cast<int64_t>(stddev))) << std::endl;
}

void merge_stats(aic::Program::ExecutionStats& into, const aic::Program::ExecutionStats& from) {
    for (const auto& [op, count] : from.opcode_counts) {
        into.opcode_counts[op] += count;
    }
    for (const auto& [op, dt] : from.opcode_time) {
        into.opcode_time[op] += dt;
    }
    for (const auto& [fn, dt] : from.function_time) {
        into.function_time[fn] += dt;
    }
    for (const auto& [fn, count] : from.function_instruction_counts) {
        into.function_instruction_counts[fn] += count;
    }
}

void print_execution_stats(const aic::Program::ExecutionStats& stats, bool show_profile, bool show_opcode_stats) {
    if (show_profile) {
        std::cout << "Profile: function timing" << std::endl;
        for (const auto& [fn, total] : stats.function_time) {
            const size_t count = stats.function_instruction_counts.contains(fn)
                                     ? stats.function_instruction_counts.at(fn)
                                     : 0;
            std::cout << "  " << fn
                      << " total=" << format_duration(total)
                      << " instructions=" << count
                      << std::endl;
        }

        std::cout << "Profile: opcode timing" << std::endl;
        for (const auto& [op, total] : stats.opcode_time) {
            std::cout << "  " << aic::operation_kind_to_string(op)
                      << " total=" << format_duration(total)
                      << std::endl;
        }
    }

    if (show_opcode_stats) {
        std::cout << "Opcode stats (execution counts)" << std::endl;
        for (const auto& [op, count] : stats.opcode_counts) {
            std::cout << "  " << aic::operation_kind_to_string(op)
                      << ": " << count
                      << std::endl;
        }
    }
}

} // namespace

int main(int argc, char** argv) {
    try {
        CliOptions options = parse_cli(argc, argv);
        if (options.show_help) {
            print_help();
            return 0;
        }

        aic::register_operations();

        if (options.json_mode != JsonMode::None) {
            std::vector<std::string> json_files;
            bool has_errors = false;
            std::cout << "{\"files\":[";
            for (size_t file_index = 0; file_index < options.input_files.size(); file_index++) {
                const std::string& filename = options.input_files[file_index];
                if (file_index > 0) {
                    std::cout << ",";
                }

                std::vector<aic::Token> tokens;
                std::vector<aic::VerificationDiagnostic> verification;
                std::optional<aic::Program> vm;
                try {
                    std::string in = read_file(filename);
                    tokens = aic::tokenize(in);
                    if (options.json_mode != JsonMode::Tokens) {
                        vm = load_program_with_modules(filename);
                        aic::optimize_program(*vm);
                        verification = aic::verify(*vm);
                        has_errors = has_errors || aic::has_verification_errors(verification);
                    }
                    print_json_file(std::cout,
                                    filename,
                                    options.json_mode,
                                    tokens,
                                    vm.has_value() ? &*vm : nullptr,
                                    verification);
                } catch (const std::exception& err) {
                    has_errors = true;
                    std::cout << "{"
                              << "\"file\":" << json_string(filename)
                              << ",\"valid\":false"
                              << ",\"diagnostics\":[{\"severity\":\"error\",\"message\":"
                              << json_string(err.what())
                              << ",\"function\":\"\",\"opcode\":\"\",\"pc\":null}]"
                              << "}";
                }
            }
            std::cout << "]}";
            std::cout << std::endl;
            return has_errors ? 1 : 0;
        }

        for (const std::string& filename : options.input_files) {
            std::cout << "=== Running: " << filename << " ===" << std::endl;
            if (options.dump_tokens) {
                std::string in = read_file(filename);
                std::vector<aic::Token> tokens = aic::tokenize(in);
                aic::print_tokens(tokens);
            }

            aic::Program vm = load_program_with_modules(filename);
            aic::optimize_program(vm);
            std::vector<aic::VerificationDiagnostic> verification = aic::verify(vm);
            for (const aic::VerificationDiagnostic& diagnostic : verification) {
                std::cerr << aic::format_verification_diagnostic(diagnostic) << std::endl;
            }
            if (aic::has_verification_errors(verification)) {
                return 1;
            }

            if (options.dump_bytecode) {
                print_bytecode(vm);
            }
            if (options.dump_constants) {
                print_constants(vm);
            }
            if (options.dump_functions) {
                print_functions(vm);
            }
            if (options.dump_symbols) {
                print_symbols(vm);
            }
            if (options.dump_labels) {
                print_labels(vm);
            }

            if (options.seed.has_value()) {
                aic::Program::rng.seed(*options.seed);
            }

            aic::Program::ExecutionOptions exec_options{};
            exec_options.profile = options.profile;
            exec_options.opcode_stats = options.opcode_stats;
            exec_options.trace_time = options.trace_time;
            aic::Program::ExecutionStats aggregate_stats{};
            exec_options.stats = &aggregate_stats;

            if (options.benchmark) {
                for (size_t i = 0; i < options.warmup; i++) {
                    aic::Program warm_vm = vm;
                    aic::Program::ExecutionStats warmup_stats{};
                    exec_options.stats = &warmup_stats;
                    warm_vm.exec(exec_options);
                }

                std::vector<std::chrono::nanoseconds> samples;
                samples.reserve(options.runs);

                for (size_t i = 0; i < options.runs; i++) {
                    aic::Program run_vm = vm;
                    aic::Program::ExecutionStats run_stats{};
                    exec_options.stats = &run_stats;
                    const auto t0 = std::chrono::steady_clock::now();
                    run_vm.exec(exec_options);
                    const auto t1 = std::chrono::steady_clock::now();
                    samples.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0));
                    merge_stats(aggregate_stats, run_stats);
                }

                print_benchmark_stats(samples);
                print_execution_stats(aggregate_stats, options.profile, options.opcode_stats);
                continue;
            }

            const auto t0 = std::chrono::steady_clock::now();
            vm.exec(exec_options);
            const auto t1 = std::chrono::steady_clock::now();

            if (options.time) {
                auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
                std::cout << "Program executed in " << format_duration(dt) << std::endl;
            }
            print_execution_stats(aggregate_stats, options.profile, options.opcode_stats);
        }

        return 0;
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
