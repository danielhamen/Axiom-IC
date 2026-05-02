#include "core/operations.hpp"
#include "parser/parser.hpp"
#include "parser/tokenizer.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
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
#include <vector>

namespace {

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
        throw std::runtime_error("Could not open file: " + path);
    }

    std::string in;
    std::string line;
    while (std::getline(file, line)) {
        in += line + "\n";
    }

    return in;
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

        for (const std::string& filename : options.input_files) {
            std::cout << "=== Running: " << filename << " ===" << std::endl;
            std::string in = read_file(filename);

            std::vector<aic::Token> tokens = aic::tokenize(in);
            if (options.dump_tokens) {
                aic::print_tokens(tokens);
            }

            aic::Program vm = aic::parse(tokens);

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
