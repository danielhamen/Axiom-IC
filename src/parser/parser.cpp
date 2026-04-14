#include "parser.hpp"

#include "../core/operations.hpp"
#include "../util/symbols.hpp"

#include <stdexcept>
#include <string>
#include <vector>

namespace aic {

namespace {

[[noreturn]] void throw_parse_error(const Token& token, const std::string& message) {
    throw std::runtime_error("Parse error at line " + std::to_string(token.line) +
                             ", column " + std::to_string(token.column) +
                             ": " + message);
}

Value parse_immediate_literal(const Token& token) {
    Value out{};
    if (token.type == TokenType::Integer) {
        out.kind = ValueKind::Integer;
        out.i = std::stoll(token.lexeme);
        return out;
    }

    if (token.type == TokenType::Float) {
        out.kind = ValueKind::Float;
        out.f = std::stod(token.lexeme);
        return out;
    }

    if (token.type == TokenType::String) {
        out.kind = ValueKind::String;
        out.s = token.lexeme;
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

void parse_line(const std::vector<Token>& line, Program& vm, Directive& current_directive) {
    if (line.empty()) {
        return;
    }

    const Token& head_token = line.at(0);
    const std::string& head = head_token.lexeme;

    if (head_token.type == TokenType::Directive) {
        std::string directive = head.substr(1);

        if (directive == "const") {
            if (line.size() != 1) {
                throw_parse_error(head_token, "Too many elements to unpack in `.const` directive; unary directives must not take any additional tokens");
            }

            current_directive = Directive::Constant;
            return;
        }

        if (directive == "include") {
            current_directive = Directive::Include;
            throw_parse_error(head_token, "`include` directives are not yet implemented");
        }

        current_directive = Directive::Function;
        vm.fc = vm.functions.size();

        std::string fn_name = directive;
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

    if (line.size() >= 2 && head_token.type == TokenType::Identifier && line.at(1).type == TokenType::Colon) {
        if (line.size() > 2) {
            throw_parse_error(line.at(2), "Too many elements to unpack in label.");
        }

        std::string label_name = head;

        Function* curr_fn = vm.functions.at(vm.fc);
        curr_fn->labels[label_name] = curr_fn->ins.size();
        return;
    }

    if (current_directive == Directive::Constant) {
        std::string type = head;
        if (type == "INT") {
            if (line.size() < 2 || line[1].type != TokenType::Integer) {
                throw_parse_error(head_token, "INT requires an integer literal");
            }

            int param = std::stoi(line[1].lexeme);
            Value v;
            v.kind = ValueKind::Integer;
            v.i = param;
            vm.constants.push_back(v);
        } else if (type == "FLOAT") {
            if (line.size() < 2 || line[1].type != TokenType::Float) {
                throw_parse_error(head_token, "FLOAT requires a float literal");
            }

            double param = std::stod(line[1].lexeme);
            Value v;
            v.kind = ValueKind::Float;
            v.f = param;
            vm.constants.push_back(v);
        } else if (type == "BOOL") {
            if (line.size() != 2 || (line[1].lexeme != "true" && line[1].lexeme != "false")) {
                throw_parse_error(head_token, "BOOL requires true or false");
            }

            bool param = line[1].lexeme == "true";
            Value v;
            v.kind = ValueKind::Boolean;
            v.b = param;
            vm.constants.push_back(v);
        } else if (type == "NULL") {
            Value v;
            v.kind = ValueKind::Null;
            vm.constants.push_back(v);
        } else if (type == "STR") {
            if (line.size() < 2 || line[1].type != TokenType::String) {
                throw_parse_error(head_token, "STR requires a string literal");
            }

            std::string param = line[1].lexeme;
            Value v;
            v.kind = ValueKind::String;
            v.s = param;
            vm.constants.push_back(v);
        } else {
            throw_parse_error(head_token, "Unknown type in constant list '" + type + "'");
        }

        return;
    }

    std::string ins_opname = head;
    auto it = operation_by_name.find(ins_opname);
    if (it == operation_by_name.end()) {
        throw_parse_error(head_token, "Unknown instruction: " + ins_opname);
    }
    const Operation& ins_op = it->second;

    Instruction ins;
    ins.op = ins_op.kind;

    size_t j = 1;
    size_t param_idx = 0;
    Operand null_op{OperandKind::None};
    ins.x = null_op;
    ins.y = null_op;
    ins.z = null_op;

    while (j < line.size()) {
        const Token& tok = line[j];
        Operand op{};

        if (tok.type == TokenType::Dollar || tok.type == TokenType::Hash || tok.type == TokenType::At || tok.type == TokenType::Amp) {
            if (j + 1 >= line.size()) {
                throw_parse_error(tok, "Missing value after operand prefix.");
            }

            const Token& next = line[j + 1];
            if (tok.type == TokenType::Dollar) {
                if (next.type != TokenType::Integer) {
                    throw_parse_error(next, "Expected integer literal after '$'");
                }
                op.kind = OperandKind::Slot;
                op.value = std::stoi(next.lexeme);
            } else if (tok.type == TokenType::Hash) {
                op.kind = OperandKind::Immediate;
                op.immediate = parse_immediate_literal(next);
                op.has_immediate = true;
            } else if (tok.type == TokenType::At) {
                if (next.type != TokenType::Integer) {
                    throw_parse_error(next, "Expected integer literal after '@'");
                }
                op.kind = OperandKind::Constant;
                op.value = std::stoi(next.lexeme);
            } else if (tok.type == TokenType::Amp) {
                if (next.type != TokenType::Integer) {
                    throw_parse_error(next, "Expected integer literal after '&'");
                }
                op.kind = OperandKind::Address;
                op.value = std::stoi(next.lexeme);
            }

            j += 2;
        }
        else if (tok.type == TokenType::Identifier && is_symbol(tok.lexeme)) {
            op.kind = OperandKind::Label;
            op.strval = tok.lexeme;
            j += 1;
        }
        else if (tok.type == TokenType::Comma) {
            param_idx++;
            j += 1;
            continue;
        }
        else if (tok.type == TokenType::Invalid) {
            throw_parse_error(tok, "Invalid token: " + tok.lexeme);
        }
        else {
            throw_parse_error(tok, "Unknown token in instruction/parameter parsing: " + tok.lexeme);
        }

        if (param_idx == 0) {
            ins.x = op;
        }
        else if (param_idx == 1) {
            ins.y = op;
        }
        else if (param_idx == 2) {
            ins.z = op;
        }
        else {
            throw_parse_error(tok, "Too many operands in instruction.");
        }
    }

    size_t expected_arity = ins_op.arity;
    size_t recieved_arity =
        (ins.x.is_none() ? 0 : 1) +
        (ins.y.is_none() ? 0 : 1) +
        (ins.z.is_none() ? 0 : 1);
    if (expected_arity != recieved_arity) {
        throw_parse_error(head_token, "Error: '" + ins_op.name + "' takes exactly " + std::to_string(expected_arity) + " arguments. Recieved " + std::to_string(recieved_arity));
    }

    vm.functions.at(vm.fc)->ins.push_back(ins);
}

} // namespace

Program parse(const std::vector<Token>& tokens) {
    Program vm;
    vm.pc = 0;
    Directive current_directive = Directive::None;

    std::vector<Token> line;
    for (const Token& tok : tokens) {
        if (tok.type == TokenType::Newline || tok.type == TokenType::EndOfFile) {
            parse_line(line, vm, current_directive);
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
