#include "program.hpp"

#include <iostream>
#include <stdexcept>

namespace aic {

namespace {

[[noreturn]] void throw_exec_error(Program& program, const std::string& message, const Instruction* ins = nullptr) {
    std::string fn_name;
    if (program.fc < program.functions.size()) {
        fn_name = program.functions.at(program.fc)->name;
    }

    std::string opcode;
    if (ins != nullptr) {
        opcode = operation_kind_to_string(ins->op);
    }

    throw std::runtime_error(format_error_context(ErrorPhase::Exec,
                                                  message,
                                                  -1,
                                                  -1,
                                                  fn_name,
                                                  opcode,
                                                  program.pc));
}

} // namespace

Value& Program::slot(size_t index) {
    if (index >= memory.size()) {
        memory.resize(index + 1);
    }
    return memory[index];
}

void Program::resolve(const Instruction& ins) {
    const OperationKind& op = ins.op;
    const Operand& x = ins.x;
    const Operand& y = ins.y;
    const Operand& z = ins.z;

    switch (op) {
    case OperationKind::ADD: {
        Value lhs = read_operand_strict(y, ValueKind::Integer);
        Value rhs = read_operand_strict(z, ValueKind::Integer);

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = lhs.i + rhs.i;

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MUL: {
        Value lhs = read_operand_strict(y, ValueKind::Integer);
        Value rhs = read_operand_strict(z, ValueKind::Integer);

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = lhs.i * rhs.i;

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::SUB: {
        Value lhs = read_operand_strict(y, ValueKind::Integer);
        Value rhs = read_operand_strict(z, ValueKind::Integer);

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = lhs.i - rhs.i;

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::DIV: {
        Value lhs = read_operand_strict(y, ValueKind::Integer);
        Value rhs = read_operand_strict(z, ValueKind::Integer);

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = lhs.i / rhs.i;

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MOD: {
        Value lhs = read_operand_strict(y, ValueKind::Integer);
        Value rhs = read_operand_strict(z, ValueKind::Integer);

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = lhs.i % rhs.i;

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::HALT: {
        halted = true;
        return;
    }
    case OperationKind::JMP: {
        if (x.kind != OperandKind::Label) {
            throw_exec_error(*this,
                             "Operand kind mismatch: expected Label for JMP destination, actual " + x.kindstr(),
                             &ins);
        }

        const auto& labels = functions.at(fc)->labels;
        const std::string& label_name = x.strval;

        if (!labels.contains(label_name))
            throw_exec_error(*this,
                             "Undefined label reference '" + label_name + "' in current function scope",
                             &ins);

        size_t dst_pc = labels.at(label_name);
        pc = dst_pc;

        return;
    }
    case OperationKind::PRINT: {
        if (x.is_none() || !y.is_none() || !z.is_none())
            throw_exec_error(*this,
                             "Operand count mismatch for PRINT: expected exactly one operand",
                             &ins);

        Value v = read_operand(x);
        std::cout << v.to_str();

        pc++;
        return;
    }
    default:
        throw_exec_error(*this, "Unhandled operation in resolve()", &ins);
    }
}

void Program::exec() {
    if (!functions.exists("main")) {
        throw std::runtime_error(format_error_context(ErrorPhase::Exec,
                                                      "Missing '.main' entry point"));
    }

    fc = functions.index_of("main");

    while (!halted) {
        const Instruction& ins = functions.at(fc)->ins.at(pc);
        resolve(ins);
    }
}

Value Program::read_operand(const Operand& op) {
    Value v{};

    switch (op.kind) {
        case OperandKind::None:
            v.kind = ValueKind::Null;
            break;
        case OperandKind::Slot:
            v = slot(op.value);
            break;
        case OperandKind::Immediate:
            if (op.has_immediate) {
                v = op.immediate;
            } else {
                v.kind = ValueKind::Integer;
                v.i = op.value;
            }
            break;
        case OperandKind::Constant:
            if (op.value >= constants.size()) {
                throw_exec_error(*this,
                                 "Constant index out of bounds: requested " + std::to_string(op.value) +
                                     ", constant count " + std::to_string(constants.size()));
            }

            v = constants.at(op.value);
            break;
        default:
            throw_exec_error(*this,
                             "Unsupported operand kind in read_operand: " + op.kindstr());
    }

    return v;
}

Value Program::read_operand_strict(const Operand& op, ValueKind enforced_type) {
    Value v = read_operand(op);
    if (v.kind != enforced_type) {
        throw_exec_error(*this,
                         "Type mismatch: expected " + value_kind_to_string(enforced_type) +
                             ", actual " + value_kind_to_string(v.kind));
    }

    return v;
}

void Program::write_operand(const Operand& op, const Value& v) {
    if (op.kind != OperandKind::Slot) {
        throw_exec_error(*this,
                         "Operand kind mismatch: expected Slot destination, actual " + op.kindstr());
    }

    slot(op.value) = v;
}

} // namespace aic
