#include "program.hpp"

#include <iostream>
#include <stdexcept>

namespace aic {

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
            throw std::runtime_error("Expected Label after JMP");
        }

        const auto& labels = functions.at(fc)->labels;
        const std::string& label_name = x.strval;

        if (!labels.contains(label_name))
            throw std::runtime_error("Reference to label that does not exist in the current scope");

        size_t dst_pc = labels.at(label_name);
        pc = dst_pc;

        return;
    }
    case OperationKind::PRINT: {
        if (x.is_none() || !y.is_none() || !z.is_none())
            throw std::runtime_error("PRINT must have exactly one parameter.");

        Value v = read_operand(x);
        std::cout << v.to_str();

        pc++;
        return;
    }
    default:
        throw std::runtime_error("Unmatched operation found in resolve()");
    }
}

void Program::exec() {
    if (!functions.exists("main")) {
        throw std::runtime_error("Error no '.main' entry point");
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
            v.kind = ValueKind::Integer;
            v.i = op.value;
            break;
        case OperandKind::Constant:
            if (op.value >= constants.size()) {
                throw std::runtime_error("Cannot index constant map at index " +
                    std::to_string(op.value) +
                    " as it exceeds the bounds of the constant vector.");
            }

            v = constants.at(op.value);
            break;
        default:
            throw std::runtime_error("Operand cannot be resolved in read_operand");
    }

    return v;
}

Value Program::read_operand_strict(const Operand& op, ValueKind enforced_type) {
    Value v = read_operand(op);
    if (v.kind != enforced_type) {
        throw std::runtime_error("Unexepected type.");
    }

    return v;
}

void Program::write_operand(const Operand& op, const Value& v) {
    if (op.kind != OperandKind::Slot) {
        throw std::runtime_error("write_operand requires a slot destination");
    }

    slot(op.value) = v;
}

} // namespace aic
