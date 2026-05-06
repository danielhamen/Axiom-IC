#include "parser.hpp"

#include "../core/operations.hpp"
#include "../util/symbols.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace aic {

namespace {

enum class ParseMode {
    None,
    ConstSection,
    FunctionBody
};

[[noreturn]] void throw_parse_error(const Token& token, const std::string& message) {
    throw std::runtime_error(format_error_context(ErrorPhase::Parse,
                                                  message,
                                                  static_cast<int>(token.line),
                                                  static_cast<int>(token.column)));
}

int64_t parse_int64_literal(const Token& token, const std::string& usage) {
    try {
        return std::stoll(token.lexeme);
    } catch (const std::invalid_argument&) {
        throw_parse_error(token, "Invalid integer literal for " + usage + ": '" + token.lexeme + "'");
    } catch (const std::out_of_range&) {
        throw_parse_error(token, "Integer literal out of range for " + usage + ": '" + token.lexeme + "'");
    }
}

int64_t parse_non_negative_index(const Token& token, const std::string& usage) {
    int64_t value = parse_int64_literal(token, usage);
    if (value < 0) {
        throw_parse_error(token, usage + " must be non-negative, got '" + token.lexeme + "'");
    }
    return value;
}

double parse_double_literal(const Token& token, const std::string& usage) {
    try {
        return std::stod(token.lexeme);
    } catch (const std::invalid_argument&) {
        throw_parse_error(token, "Invalid float literal for " + usage + ": '" + token.lexeme + "'");
    } catch (const std::out_of_range&) {
        throw_parse_error(token, "Float literal out of range for " + usage + ": '" + token.lexeme + "'");
    }
}

StringFlavor string_flavor_from_token(const Token& token) {
    switch (token.string_kind) {
        case StringTokenKind::Normal:
            return StringFlavor::Normal;
        case StringTokenKind::SQL:
            return StringFlavor::SQL;
        case StringTokenKind::Regex:
            return StringFlavor::Regex;
        case StringTokenKind::Format:
            return StringFlavor::Format;
    }
    return StringFlavor::Normal;
}

Value parse_immediate_literal(const Token& token) {
    Value out{};
    if (token.type == TokenType::Integer) {
        out.kind = ValueKind::Integer;
        out.i = parse_int64_literal(token, "immediate integer");
        return out;
    }

    if (token.type == TokenType::Float) {
        out.kind = ValueKind::Float;
        out.f = parse_double_literal(token, "immediate float");
        return out;
    }

    if (token.type == TokenType::String) {
        out.kind = ValueKind::String;
        out.s = token.lexeme;
        out.string_flavor = string_flavor_from_token(token);
        return out;
    }

    if (token.type == TokenType::Identifier && token.lexeme == "true") {
        out.kind = ValueKind::Boolean;
        out.b = true;
        return out;
    }

    if (token.type == TokenType::Identifier && token.lexeme == "false") {
        out.kind = ValueKind::Boolean;
        out.b = false;
        return out;
    }

    if (token.type == TokenType::Identifier && (token.lexeme == "NULL" || token.lexeme == "null")) {
        out.kind = ValueKind::Null;
        return out;
    }

    throw_parse_error(token, "Unsupported immediate literal after '#': " + token.lexeme);
}

bool is_const_declaration(const std::string& token) {
    return token == "INT" || token == "FLOAT" || token == "BOOL" || token == "NULL" || token == "STR";
}

size_t write_constant(Program& vm, std::optional<size_t> index, const Value& value) {
    if (!index.has_value()) {
        vm.constants.push_back(value);
        return vm.constants.size() - 1;
    }

    if (*index >= vm.constants.size()) {
        Value null_value;
        null_value.kind = ValueKind::Null;
        vm.constants.resize(*index + 1, null_value);
    }
    vm.constants[*index] = value;
    return *index;
}

size_t parse_constant_declaration(const std::vector<Token>& line,
                                  size_t offset,
                                  Program& vm,
                                  const std::string& usage,
                                  const std::string& current_pool = "") {
    std::optional<size_t> explicit_index;
    if (offset < line.size() && line[offset].type == TokenType::Integer) {
        int64_t parsed_index = parse_non_negative_index(line[offset], usage + " constant index");
        explicit_index = static_cast<size_t>(parsed_index);
        offset++;
    }

    if (offset >= line.size()) {
        throw_parse_error(line.back(), usage + " requires a constant type");
    }

    const Token& type_token = line.at(offset);
    const std::string& type = type_token.lexeme;
    if (type == "INT") {
        if (line.size() != offset + 2 || line[offset + 1].type != TokenType::Integer) {
            throw_parse_error(type_token, usage + " INT requires exactly one integer literal");
        }

        Value v;
        v.kind = ValueKind::Integer;
        v.i = parse_int64_literal(line[offset + 1], usage + " INT");
        size_t written = write_constant(vm, explicit_index, v);
        if (!current_pool.empty()) {
            ConstantPoolRange& pool = vm.constant_pools[current_pool];
            if (written < pool.start) {
                throw_parse_error(type_token, "Named constant pool entries cannot be written before the pool start");
            }
            pool.count = std::max(pool.count, written - pool.start + 1);
        }
        return written;
    }

    if (type == "FLOAT") {
        if (line.size() != offset + 2 || line[offset + 1].type != TokenType::Float) {
            throw_parse_error(type_token, usage + " FLOAT requires exactly one float literal");
        }

        Value v;
        v.kind = ValueKind::Float;
        v.f = parse_double_literal(line[offset + 1], usage + " FLOAT");
        size_t written = write_constant(vm, explicit_index, v);
        if (!current_pool.empty()) {
            ConstantPoolRange& pool = vm.constant_pools[current_pool];
            if (written < pool.start) {
                throw_parse_error(type_token, "Named constant pool entries cannot be written before the pool start");
            }
            pool.count = std::max(pool.count, written - pool.start + 1);
        }
        return written;
    }

    if (type == "BOOL") {
        if (line.size() != offset + 2 ||
            (line[offset + 1].lexeme != "true" && line[offset + 1].lexeme != "false")) {
            throw_parse_error(type_token, usage + " BOOL requires exactly one boolean literal");
        }

        Value v;
        v.kind = ValueKind::Boolean;
        v.b = line[offset + 1].lexeme == "true";
        size_t written = write_constant(vm, explicit_index, v);
        if (!current_pool.empty()) {
            ConstantPoolRange& pool = vm.constant_pools[current_pool];
            if (written < pool.start) {
                throw_parse_error(type_token, "Named constant pool entries cannot be written before the pool start");
            }
            pool.count = std::max(pool.count, written - pool.start + 1);
        }
        return written;
    }

    if (type == "NULL") {
        if (line.size() != offset + 1) {
            throw_parse_error(type_token, usage + " NULL does not take a value");
        }

        Value v;
        v.kind = ValueKind::Null;
        size_t written = write_constant(vm, explicit_index, v);
        if (!current_pool.empty()) {
            ConstantPoolRange& pool = vm.constant_pools[current_pool];
            if (written < pool.start) {
                throw_parse_error(type_token, "Named constant pool entries cannot be written before the pool start");
            }
            pool.count = std::max(pool.count, written - pool.start + 1);
        }
        return written;
    }

    if (type == "STR") {
        if (line.size() != offset + 2 || line[offset + 1].type != TokenType::String) {
            throw_parse_error(type_token, usage + " STR requires exactly one string literal");
        }

        Value v;
        v.kind = ValueKind::String;
        v.s = line[offset + 1].lexeme;
        v.string_flavor = string_flavor_from_token(line[offset + 1]);
        size_t written = write_constant(vm, explicit_index, v);
        if (!current_pool.empty()) {
            ConstantPoolRange& pool = vm.constant_pools[current_pool];
            if (written < pool.start) {
                throw_parse_error(type_token, "Named constant pool entries cannot be written before the pool start");
            }
            pool.count = std::max(pool.count, written - pool.start + 1);
        }
        return written;
    }

    throw_parse_error(type_token, "Unknown constant type '" + type + "'");
}

bool try_parse_argument(const Token& token, int64_t& out_index) {
    if (token.type != TokenType::Identifier) {
        return false;
    }

    const std::string& lexeme = token.lexeme;
    if (!lexeme.starts_with("arg") || lexeme.size() <= 3) {
        return false;
    }

    for (size_t i = 3; i < lexeme.size(); i++) {
        if (!std::isdigit(static_cast<unsigned char>(lexeme[i]))) {
            return false;
        }
    }

    try {
        out_index = std::stoll(lexeme.substr(3));
    } catch (const std::invalid_argument&) {
        throw_parse_error(token, "Invalid argument index token: " + lexeme);
    } catch (const std::out_of_range&) {
        throw_parse_error(token, "Argument index out of range: " + lexeme);
    }
    return true;
}

std::string parse_function_name_token(const Token& token) {
    if (token.type == TokenType::Directive) {
        return token.lexeme.substr(1);
    }
    if (token.type == TokenType::Identifier) {
        return token.lexeme;
    }
    throw_parse_error(token, "Expected function name");
}

std::unordered_set<std::string> operation_categories() {
    std::unordered_set<std::string> categories;
    for (const OperationDefinition& definition : operation_definitions()) {
        categories.insert(definition.category);
    }
    return categories;
}

bool is_valid_operation_category(const std::string& category) {
    static const std::unordered_set<std::string> categories = operation_categories();
    return categories.contains(category);
}

std::string parse_import_category(const std::vector<Token>& line) {
    if (line.size() != 4 ||
        line[1].type != TokenType::Less ||
        line[2].type != TokenType::Identifier ||
        line[3].type != TokenType::Greater) {
        throw_parse_error(line[0], "`.import` requires syntax `.import <category>`");
    }

    const std::string& category = line[2].lexeme;
    if (!is_valid_operation_category(category)) {
        throw_parse_error(line[2], "Unknown operation category import: " + category);
    }
    return category;
}

bool is_module_path_separator(TokenType type) {
    return type == TokenType::Backslash || type == TokenType::Slash;
}

std::vector<std::string> parse_module_path(const std::vector<Token>& line, size_t& index) {
    std::vector<std::string> path;
    bool expect_segment = true;

    while (index < line.size()) {
        const Token& token = line[index];
        if (expect_segment) {
            if (token.type != TokenType::Identifier || !is_symbol(token.lexeme)) {
                throw_parse_error(token, "Expected module path segment");
            }
            path.push_back(token.lexeme);
            index++;
            expect_segment = false;
            continue;
        }

        if (!is_module_path_separator(token.type)) {
            break;
        }

        index++;
        expect_segment = true;
    }

    if (path.empty() || expect_segment) {
        throw_parse_error(line[index - 1], "Module import path cannot be empty or end with a separator");
    }

    return path;
}

ModuleImport parse_module_import(const std::vector<Token>& line) {
    if (line.size() < 2) {
        throw_parse_error(line[0], "`.import` requires a category import or module path");
    }

    size_t index = 1;
    ModuleImport import;
    import.line = line[0].line;
    import.column = line[0].column;
    import.path = parse_module_path(line, index);

    if (index == line.size()) {
        return import;
    }

    if (line[index].type != TokenType::LeftBrace) {
        throw_parse_error(line[index], "Unexpected token in module import: " + line[index].lexeme);
    }
    index++;

    while (index < line.size()) {
        const Token& token = line[index];
        if (token.type == TokenType::RightBrace) {
            index++;
            if (index != line.size()) {
                throw_parse_error(line[index], "Unexpected token after selective import list");
            }
            if (import.selected_symbols.empty()) {
                throw_parse_error(token, "Selective import list cannot be empty");
            }
            return import;
        }

        if (token.type != TokenType::Identifier || !is_symbol(token.lexeme)) {
            throw_parse_error(token, "Expected exported symbol name in selective import");
        }
        if (std::find(import.selected_symbols.begin(), import.selected_symbols.end(), token.lexeme) !=
            import.selected_symbols.end()) {
            throw_parse_error(token, "Duplicate selective import symbol: " + token.lexeme);
        }
        import.selected_symbols.push_back(token.lexeme);
        index++;

        if (index >= line.size()) {
            break;
        }
        if (line[index].type == TokenType::Comma) {
            index++;
            if (index >= line.size() || line[index].type == TokenType::RightBrace) {
                throw_parse_error(line[index - 1], "Selective import list cannot end with a comma");
            }
            continue;
        }
        if (line[index].type == TokenType::RightBrace) {
            continue;
        }
        throw_parse_error(line[index], "Expected comma or closing brace in selective import");
    }

    throw_parse_error(line.back(), "Unterminated selective import list");
}

void parse_line(const std::vector<Token>& line,
                Program& vm,
                Directive& current_directive,
                ParseMode& parse_mode,
                bool& imports_closed,
                bool& module_declaration_allowed,
                std::string& current_constant_pool) {
    if (line.empty()) {
        return;
    }

    const Token& head_token = line.at(0);
    const std::string& head = head_token.lexeme;

    if (head_token.type == TokenType::Directive) {
        std::string directive = head.substr(1);

        if (directive == "import") {
            if (imports_closed) {
                throw_parse_error(head_token, "`.import` directives must appear at the top of the file");
            }

            current_directive = Directive::Import;
            parse_mode = ParseMode::None;
            current_constant_pool.clear();
            if (line.size() >= 2 && line[1].type == TokenType::Less) {
                vm.imported_categories.insert(parse_import_category(line));
            } else {
                vm.module_imports.push_back(parse_module_import(line));
            }
            return;
        }

        if (directive == "module") {
            if (!module_declaration_allowed || parse_mode != ParseMode::None) {
                throw_parse_error(head_token, "`.module` must appear at the top level before functions or constants");
            }
            if (line.size() != 2 || line[1].type != TokenType::Identifier || !is_symbol(line[1].lexeme)) {
                throw_parse_error(head_token, "`.module` requires exactly one module name");
            }
            if (!vm.module_name.empty()) {
                throw_parse_error(head_token, "Module already declared: " + vm.module_name);
            }

            current_directive = Directive::Module;
            parse_mode = ParseMode::None;
            current_constant_pool.clear();
            vm.module_name = line[1].lexeme;
            return;
        }

        imports_closed = true;

        if (directive == "const") {
            if (line.size() == 1) {
                current_directive = Directive::Constant;
                parse_mode = ParseMode::ConstSection;
                current_constant_pool.clear();
                module_declaration_allowed = false;
                return;
            }

            if (line.size() == 2 &&
                line[1].type == TokenType::Identifier &&
                !is_const_declaration(line[1].lexeme)) {
                const std::string& pool_name = line[1].lexeme;
                if (!is_symbol(pool_name)) {
                    throw_parse_error(line[1], "Invalid constant pool name: " + pool_name);
                }
                if (vm.constant_pools.contains(pool_name)) {
                    throw_parse_error(line[1], "Constant pool already defined: " + pool_name);
                }
                ConstantPoolRange pool;
                pool.name = pool_name;
                pool.start = vm.constants.size();
                pool.count = 0;
                vm.constant_pools.emplace(pool_name, pool);

                current_directive = Directive::Constant;
                parse_mode = ParseMode::ConstSection;
                current_constant_pool = pool_name;
                module_declaration_allowed = false;
                return;
            }

            if (line.size() > 2 &&
                line[1].type == TokenType::Identifier &&
                !is_const_declaration(line[1].lexeme)) {
                throw_parse_error(line[1], "Named constant pools must use section syntax; inline named constants are invalid");
            }

            if (line.size() >= 2 &&
                ((line[1].type == TokenType::Identifier && is_const_declaration(line[1].lexeme)) ||
                 line[1].type == TokenType::Integer)) {
                parse_constant_declaration(line, 1, vm, ".const");
                current_directive = Directive::Constant;
                parse_mode = ParseMode::ConstSection;
                current_constant_pool.clear();
                module_declaration_allowed = false;
                return;
            }

            throw_parse_error(head_token, "`.const` requires either no operands or an inline constant declaration like `.const FLOAT 3.14159`");
        }

        if (directive == "include") {
            current_directive = Directive::Include;
            parse_mode = ParseMode::None;
            current_constant_pool.clear();
            module_declaration_allowed = false;
            throw_parse_error(head_token, "`include` directives are not yet implemented");
        }

        if (directive == "export") {
            if (line.size() != 2 || line[1].type != TokenType::Identifier || !is_symbol(line[1].lexeme)) {
                throw_parse_error(head_token, "`.export` requires exactly one symbol name");
            }
            if (vm.exported_symbols.contains(line[1].lexeme)) {
                throw_parse_error(line[1], "Symbol already exported: " + line[1].lexeme);
            }

            current_directive = Directive::Export;
            parse_mode = ParseMode::None;
            current_constant_pool.clear();
            module_declaration_allowed = false;
            vm.exported_symbols.insert(line[1].lexeme);
            return;
        }

        if (directive == "fn") {
            if (line.size() != 2) {
                throw_parse_error(head_token, "`.fn` requires exactly one function name");
            }

            std::string fn_name = parse_function_name_token(line.at(1));
            if (!is_symbol(fn_name)) {
                throw_parse_error(line.at(1), "Invalid function name: " + fn_name);
            }
            if (vm.functions.exists(fn_name)) {
                throw_parse_error(line.at(1), "Function already defined: " + fn_name);
            }

            current_directive = Directive::Function;
            parse_mode = ParseMode::FunctionBody;
            vm.fc = vm.functions.size();
            current_constant_pool.clear();
            module_declaration_allowed = false;

            std::vector<Instruction> fn_ins;
            std::unordered_map<std::string, size_t> fn_labels;

            Function fn{
                fn_ins,
                fn_name,
                fn_labels
            };
            vm.functions.insert(fn);
            return;
        }

        current_directive = Directive::Function;
        parse_mode = ParseMode::FunctionBody;
        vm.fc = vm.functions.size();
        current_constant_pool.clear();
        module_declaration_allowed = false;

        std::string fn_name = directive;
        if (vm.functions.exists(fn_name)) {
            throw_parse_error(head_token, "Function already defined: " + fn_name);
        }
        std::vector<Instruction> fn_ins;
        std::unordered_map<std::string, size_t> fn_labels;

        Function fn{
            fn_ins,
            fn_name,
            fn_labels
        };
        vm.functions.insert(fn);
        return;
    }

    imports_closed = true;
    module_declaration_allowed = false;

    if (line.size() >= 2 && head_token.type == TokenType::Identifier && line.at(1).type == TokenType::Colon) {
        if (current_directive != Directive::Function || parse_mode != ParseMode::FunctionBody) {
            throw_parse_error(head_token, "Label declaration is only valid inside a function body");
        }

        if (line.size() > 2) {
            throw_parse_error(line.at(2), "Too many elements to unpack in label.");
        }

        std::string label_name = head;

        Function* curr_fn = vm.functions.at(vm.fc);
        if (curr_fn->labels.find(label_name) != curr_fn->labels.end()) {
            throw_parse_error(head_token, "Label redefinition in function '" + curr_fn->name + "': " + label_name);
        }
        curr_fn->labels[label_name] = curr_fn->ins.size();
        return;
    }

    if (parse_mode == ParseMode::ConstSection) {
        parse_constant_declaration(line, 0, vm, ".const", current_constant_pool);
        return;
    }

    if (parse_mode == ParseMode::FunctionBody && is_const_declaration(head)) {
        throw_parse_error(head_token, "Constant declarations are only valid inside the `.const` section");
    }

    if (current_directive != Directive::Function || parse_mode != ParseMode::FunctionBody) {
        throw_parse_error(head_token, "Instruction encountered before any function directive");
    }

    std::string ins_opname = head;
    auto it = operation_by_name.find(ins_opname);
    if (it == operation_by_name.end()) {
        throw_parse_error(head_token, "Unknown instruction: " + ins_opname);
    }
    const Operation& ins_op = it->second;
    if (!vm.imported_categories.contains(ins_op.category)) {
        throw_parse_error(head_token,
                          "Instruction '" + ins_op.name + "' requires `.import <" +
                              ins_op.category + ">` at the top of the file");
    }

    Instruction ins;
    ins.op = ins_op.kind;

    Operand null_op{OperandKind::None};
    ins.x = null_op;
    ins.y = null_op;
    ins.z = null_op;
    std::vector<Operand> parsed_operands;
    size_t j = 1;

    auto parse_operand_at = [&](size_t& index) -> Operand {
        const Token& tok = line[index];
        Operand op{};

        if (tok.type == TokenType::Dollar || tok.type == TokenType::Hash || tok.type == TokenType::At || tok.type == TokenType::Amp) {
            if (index + 1 >= line.size()) {
                throw_parse_error(tok, "Missing value after operand prefix.");
            }

            const Token& next = line[index + 1];
            if (tok.type == TokenType::Dollar) {
                if (next.type != TokenType::Integer) {
                    throw_parse_error(next, "Expected integer literal after '$'");
                }
                op.kind = OperandKind::Slot;
                op.value = parse_non_negative_index(next, "Slot index");
            } else if (tok.type == TokenType::Hash) {
                op.kind = OperandKind::Immediate;
                op.immediate = parse_immediate_literal(next);
                op.has_immediate = true;
            } else if (tok.type == TokenType::At) {
                if (next.type != TokenType::Integer) {
                    throw_parse_error(next, "Expected integer literal after '@'");
                }
                op.kind = OperandKind::Constant;
                op.value = parse_non_negative_index(next, "Constant index");
            } else if (tok.type == TokenType::Amp) {
                if (next.type != TokenType::Integer) {
                    throw_parse_error(next, "Expected integer literal after '&'");
                }
                op.kind = OperandKind::Address;
                op.value = parse_non_negative_index(next, "Address index");
            }

            index += 2;
            return op;
        }

        if (tok.type == TokenType::Directive) {
            if (ins.op != OperationKind::CALL) {
                throw_parse_error(tok, "Directive operands are only valid for CALL");
            }

            op.kind = OperandKind::Function;
            op.strval = tok.lexeme.substr(1);
            index += 1;
            return op;
        }

        if (tok.type == TokenType::Identifier && is_symbol(tok.lexeme)) {
            int64_t arg_index = -1;
            if (try_parse_argument(tok, arg_index)) {
                op.kind = OperandKind::Argument;
                op.value = arg_index;
            } else if (ins.op == OperationKind::CALL) {
                op.kind = OperandKind::Function;
                op.strval = tok.lexeme;
            } else {
                op.kind = OperandKind::Label;
                op.strval = tok.lexeme;
            }
            index += 1;
            return op;
        }

        if (tok.type == TokenType::Comma) {
            throw_parse_error(tok, "Unexpected comma in operand list.");
        }

        if (tok.type == TokenType::Invalid) {
            throw_parse_error(tok, "Invalid token: " + tok.lexeme);
        }

        throw_parse_error(tok, "Unknown token in instruction/parameter parsing: " + tok.lexeme);
    };

    if (j < line.size()) {
        if (line[j].type == TokenType::Comma) {
            throw_parse_error(line[j], "Operand list cannot start with a comma.");
        }

        while (j < line.size()) {
            parsed_operands.push_back(parse_operand_at(j));

            if (j >= line.size()) {
                break;
            }

            const Token& separator = line[j];
            if (separator.type != TokenType::Comma) {
                throw_parse_error(separator, "Unexpected token after final operand: " + separator.lexeme);
            }

            j += 1;
            if (j >= line.size()) {
                throw_parse_error(separator, "Operand list cannot end with a comma.");
            }
            if (line[j].type == TokenType::Comma) {
                throw_parse_error(line[j], "Consecutive commas are not allowed in operand list.");
            }
        }
    }

    if (parsed_operands.size() > 0) {
        ins.x = parsed_operands[0];
    }
    if (parsed_operands.size() > 1) {
        ins.y = parsed_operands[1];
    }
    if (parsed_operands.size() > 2) {
        ins.z = parsed_operands[2];
    }
    ins.operands = parsed_operands;

    size_t expected_arity = ins_op.arity;
    size_t received_arity = parsed_operands.size();
    if (ins.op == OperationKind::RET) {
        if (received_arity > 1) {
            throw_parse_error(head_token,
                              "Instruction 'RET' takes zero operands or one return value; received " +
                                  std::to_string(received_arity));
        }
    } else if (expected_arity == kVariadicArity) {
        size_t minimum_arity = 0;
        if (ins.op == OperationKind::STRUCT_INIT ||
            ins.op == OperationKind::LIST_DESTRUCTURE ||
            ins.op == OperationKind::STRUCT_DESTRUCTURE) {
            minimum_arity = 2;
        }
        if (received_arity < minimum_arity) {
            throw_parse_error(head_token,
                              "Instruction '" + ins_op.name +
                                  "' takes at least " + std::to_string(minimum_arity) +
                                  " operand(s); received " +
                                  std::to_string(received_arity));
        }
    } else if (expected_arity != received_arity) {
        throw_parse_error(head_token,
                          "Instruction '" + ins_op.name + "' takes exactly " +
                              std::to_string(expected_arity) + " operand(s); received " +
                              std::to_string(received_arity));
    }

    Function* curr_fn = vm.functions.at(vm.fc);
    auto update_arg_count = [&](const Operand& op) {
        if (op.kind == OperandKind::Argument) {
            size_t arg_count = static_cast<size_t>(op.value) + 1;
            if (arg_count > curr_fn->arg_count) {
                curr_fn->arg_count = arg_count;
            }
        }
    };
    for (const Operand& op : ins.operands) {
        update_arg_count(op);
    }

    curr_fn->ins.push_back(ins);
}

} // namespace

Program parse(const std::vector<Token>& tokens) {
    Program vm;
    vm.pc = 0;
    Directive current_directive = Directive::None;
    ParseMode parse_mode = ParseMode::None;
    bool imports_closed = false;
    bool module_declaration_allowed = true;
    std::string current_constant_pool;

    std::vector<Token> line;
    for (const Token& tok : tokens) {
        if (tok.type == TokenType::Newline || tok.type == TokenType::EndOfFile) {
            parse_line(line,
                       vm,
                       current_directive,
                       parse_mode,
                       imports_closed,
                       module_declaration_allowed,
                       current_constant_pool);
            line.clear();

            if (tok.type == TokenType::EndOfFile) {
                break;
            }

            continue;
        }

        line.push_back(tok);
    }

    return vm;
}

} // namespace aic
