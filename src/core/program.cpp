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

namespace {
std::runtime_error runtime_error_with_context(const Function* function, size_t pc, const std::string& message) {
    const std::string function_name = function == nullptr ? "<invalid-function>" : function->name;
    return std::runtime_error(
        std::format("Runtime error in function '{}' at pc={}: {}", function_name, pc, message));
}

size_t read_non_negative_integer_operand(Program& program,
                                         const Operand& op,
                                         const Instruction& ins,
                                         const std::string& operand_name) {
    Value v = program.read_operand_strict(op, ValueKind::Integer);
    if (v.i < 0) {
        throw_exec_error(program,
                         operand_name + " must be non-negative, got " + std::to_string(v.i),
                         &ins);
    }

    return static_cast<size_t>(v.i);
}
} // namespace

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

        Function* current_function = functions.at(fc);
        const auto& labels = current_function->labels;
        const std::string& label_name = x.strval;

        if (!labels.contains(label_name))
            throw_exec_error(*this,
                             "Undefined label reference '" + label_name + "' in current function scope",
                             &ins);

        size_t dst_pc = labels.at(label_name);
        if (dst_pc >= current_function->ins.size()) {
            throw runtime_error_with_context(
                current_function,
                pc,
                std::format("Jump to invalid label '{}' (target pc={} out of range)", label_name, dst_pc));
        }

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
    case OperationKind::PUSH: {
        Value v = read_operand(x);
        stack.push_back(v);
        pc++;
        return;
    }
    case OperationKind::POP: {
        if (stack.empty()) {
            throw_exec_error(*this, "POP attempted on an empty stack", &ins);
        }

        Value v = stack.back();
        stack.pop_back();
        write_operand(x, v);
        pc++;
        return;
    }
    case OperationKind::CALL: {
        if (x.kind != OperandKind::Function) {
            throw_exec_error(*this, "CALL expects a function operand", &ins);
        }

        if (!functions.exists(x.strval)) {
            throw_exec_error(*this, "CALL target function does not exist: " + x.strval, &ins);
        }

        size_t callee_fc = functions.index_of(x.strval);
        Function* callee = functions.at(callee_fc);
        if (callee->arg_count > stack.size()) {
            throw_exec_error(*this,
                             "Not enough arguments on stack for CALL " + x.strval +
                                 " (expected " + std::to_string(callee->arg_count) +
                                 ", got " + std::to_string(stack.size()) + ")",
                             &ins);
        }

        const size_t args_begin = stack.size() - callee->arg_count;
        std::vector<Value> args(stack.begin() + args_begin, stack.end());
        stack.resize(args_begin);

        CallFrame frame;
        frame.return_pc = pc + 1;
        frame.return_fc = fc;
        frame.args = std::move(args);
        call_stack.push_back(std::move(frame));

        fc = callee_fc;
        pc = 0;
        return;
    }
    case OperationKind::RETVAL: {
        if (call_stack.empty()) {
            throw_exec_error(*this, "RETVAL used outside of a called function", &ins);
        }

        call_stack.back().return_value = read_operand(x);
        call_stack.back().has_return_value = true;
        pc++;
        return;
    }
    case OperationKind::RET: {
        if (call_stack.empty()) {
            halted = true;
            return;
        }

        CallFrame frame = call_stack.back();
        call_stack.pop_back();
        if (frame.has_return_value) {
            stack.push_back(frame.return_value);
        }

        fc = frame.return_fc;
        pc = frame.return_pc;
        return;
    }
    case OperationKind::VNEW: {
        size_t initial_size = read_non_negative_integer_operand(*this, y, ins, "VNEW size");

        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(initial_size);
        for (auto& item : out.vec) {
            item.kind = ValueKind::Null;
        }

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VPUSH: {
        Value vec_value = read_operand_strict(x, ValueKind::Vector);
        Value element = read_operand(y);
        vec_value.vec.push_back(element);
        write_operand(x, vec_value);
        pc++;
        return;
    }
    case OperationKind::VGET: {
        Value vec_value = read_operand_strict(y, ValueKind::Vector);
        size_t index = read_non_negative_integer_operand(*this, z, ins, "VGET index");
        if (index >= vec_value.vec.size()) {
            throw_exec_error(*this,
                             "VGET index out of bounds: " + std::to_string(index) +
                                 " for vector size " + std::to_string(vec_value.vec.size()),
                             &ins);
        }

        write_operand(x, vec_value.vec[index]);
        pc++;
        return;
    }
    case OperationKind::VSET: {
        Value vec_value = read_operand_strict(x, ValueKind::Vector);
        size_t index = read_non_negative_integer_operand(*this, y, ins, "VSET index");
        if (index >= vec_value.vec.size()) {
            throw_exec_error(*this,
                             "VSET index out of bounds: " + std::to_string(index) +
                                 " for vector size " + std::to_string(vec_value.vec.size()),
                             &ins);
        }

        vec_value.vec[index] = read_operand(z);
        write_operand(x, vec_value);
        pc++;
        return;
    }
    case OperationKind::VLEN: {
        Value vec_value = read_operand_strict(y, ValueKind::Vector);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = static_cast<int64_t>(vec_value.vec.size());
        write_operand(x, out);
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

    pc = 0;
    halted = false;
    fc = functions.index_of("main");
    call_stack.clear();
    stack.clear();

    Function* main_function = functions.at(fc);
    if (main_function->ins.empty()) {
        throw std::runtime_error("Runtime error: '.main' function contains no instructions");
    }

    while (!halted) {
        if (fc >= functions.size()) {
            throw runtime_error_with_context(nullptr, pc, std::format("Invalid function index {}", fc));
        }

        Function* current_function = functions.at(fc);
        if (pc >= current_function->ins.size()) {
            throw runtime_error_with_context(
                current_function,
                pc,
                std::format("Instruction pointer out of range (function size={})", current_function->ins.size()));
        }

        const Instruction& ins = current_function->ins.at(pc);
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
        case OperandKind::Argument:
            if (call_stack.empty()) {
                throw_exec_error(*this, "Argument access attempted outside of function call frame");
            }
            if (op.value < 0 ||
                static_cast<size_t>(op.value) >= call_stack.back().args.size()) {
                throw_exec_error(*this,
                                 "Function argument index out of range: arg" + std::to_string(op.value));
            }
            v = call_stack.back().args.at(static_cast<size_t>(op.value));
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
