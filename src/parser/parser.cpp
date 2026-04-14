#include "parser.hpp"

#include "../core/operations.hpp"
#include "../util/symbols.hpp"

#include <stdexcept>
#include <string>
#include <vector>

namespace aic {

Program parse(std::vector<std::vector<std::string>> tokens) {
    Program vm;
    vm.pc = 0;
    Directive current_directive = Directive::None;

    size_t idx = 0;
    while (idx < tokens.size()) {
        const std::vector<std::string>& line = tokens[idx];

        if (line.empty()) {
            idx++;
            continue;
        }

        std::string head = line.at(0);

        if (is_str_tok_directive(head)) {
            std::string directive = head.substr(1);

            if (directive == "const") {
                if (line.size() != 1)
                    throw std::runtime_error("Too many elements to unpack in `.const` directive; unary directives must not take any additional tokens");

                current_directive = Directive::Constant;

                idx++;
                continue;
            }

            else if (directive == "include") {
                current_directive = Directive::Include;

                throw std::runtime_error("`include` directives are not yet implemented");
                idx++;
                continue;
            }

            else {
                current_directive = Directive::Function;
                vm.fc = vm.functions.size();

                std::string& fn_name = directive;
                std::vector<Instruction> fn_ins;
                std::unordered_map<std::string, size_t> fn_labels;

                Function fn{
                    fn_ins,
                    fn_name,
                    fn_labels
                };
                vm.functions.insert(fn);

                idx++;
                continue;
            }
        }

        else if (line.size() >= 2 && is_symbol(head) && line.at(1) == ":") {
            if (line.size() > 2)
                throw std::runtime_error("Too many elements to unpack in label.");

            std::string label_name = head;

            Function* curr_fn = vm.functions.at(vm.fc);
            curr_fn->labels[label_name] = curr_fn->ins.size();

            idx++;
            continue;
        }

        if (current_directive == Directive::Constant) {
            if (line.empty())
                throw std::runtime_error("Empty line found in constant definition scope");

            std::string type = line[0];
            if (type == "INT") {
                int param = std::stoi(line[1]);
                Value v;
                v.kind = ValueKind::Integer;
                v.i = param;
                vm.constants.push_back(v);
            } else if (type == "BOOL") {
                if (line.size() != 2 || (line[1] != "true" && line[1] != "false")) {
                    throw std::runtime_error("BOOL requires true or false");
                }

                bool param = line[1] == "true";
                Value v;
                v.kind = ValueKind::Boolean;
                v.b = param;
                vm.constants.push_back(v);
            } else if (type == "NULL") {
                Value v;
                v.kind = ValueKind::Null;
                vm.constants.push_back(v);
            } else if (type == "STR") {
                std::string param = line[1];
                Value v;
                v.kind = ValueKind::String;
                v.s = param;
                vm.constants.push_back(v);
            } else {
                throw std::runtime_error("Unknown type in constant list '" + type + "'");
            }

            idx++;
            continue;
        }

        std::string ins_opname = head;
        auto it = operation_by_name.find(ins_opname);
        if (it == operation_by_name.end()) {
            throw std::runtime_error("Unknown instruction: " + ins_opname);
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
            const std::string& tok = line[j];
            Operand op{};

            if (tok == "$" || tok == "#" || tok == "@" || tok == "&") {
                if (j + 1 >= line.size()) {
                    throw std::runtime_error("Missing value after operand prefix.");
                }

                const std::string& next = line[j + 1];

                if (tok == "$") {
                    op.kind = OperandKind::Slot;
                    op.value = std::stoi(next);
                } else if (tok == "#") {
                    op.kind = OperandKind::Immediate;
                    op.value = std::stoi(next);
                } else if (tok == "@") {
                    op.kind = OperandKind::Constant;
                    op.value = std::stoi(next);
                } else if (tok == "&") {
                    op.kind = OperandKind::Address;
                    op.value = std::stoi(next);
                }

                j += 2;
            }
            else if (is_symbol(tok)) {
                op.kind = OperandKind::Label;
                op.strval = tok;
                j += 1;
            }
            else if (tok == ",") {
                param_idx++;
                j += 1;
                continue;
            }
            else {
                throw std::runtime_error("Unknown token in instruction/parameter parsing: " + tok);
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
                throw std::runtime_error("Too many operands in instruction.");
            }
        }

        size_t expected_arity = ins_op.arity;
        size_t recieved_arity =
            (ins.x.is_none() ? 0 : 1) +
            (ins.y.is_none() ? 0 : 1) +
            (ins.z.is_none() ? 0 : 1);
        if (expected_arity != recieved_arity) {
            throw std::runtime_error("Error: '" + ins_op.name + "' takes exactly " + std::to_string(expected_arity) + " arguments. Recieved " + std::to_string(recieved_arity));
        }

        vm.functions.at(vm.fc)->ins.push_back(ins);

        idx++;
    }

    return vm;
}

} // namespace aic
