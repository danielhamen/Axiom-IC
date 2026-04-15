#include "program.hpp"

#include <cmath>
#include <random>
#include <iostream>
#include <stdexcept>
#include <utility>

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

/**
 * Generate random decimal from 0 and 1
 */
float random_decimal(Program& program) {
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(program.rng);
}

int64_t random_int(Program& program, int64_t min, int64_t max) {
    std::uniform_int_distribution<int64_t> dis(min, max);
    return dis(program.rng);
}

float random_range(Program& program, float min, float max) {
    std::uniform_real_distribution<float> dis(min, max);
    return dis(program.rng);
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

double numeric_value_as_double(Program& program, const Operand& op, const Instruction& ins, const std::string& usage) {
    Value v = program.read_operand(op);
    if (v.kind == ValueKind::Integer) {
        return static_cast<double>(v.i);
    }
    if (v.kind == ValueKind::Float) {
        return v.f;
    }

    throw_exec_error(program,
                     usage + " expects Integer or Float, got " + value_kind_to_string(v.kind),
                     &ins);
}

Value read_vector_strict(Program& program, const Operand& op) {
    return program.read_operand_strict(op, ValueKind::Vector);
}

Value read_matrix_strict(Program& program, const Operand& op) {
    return program.read_operand_strict(op, ValueKind::Matrix);
}

Value read_string_strict(Program& program, const Operand& op) {
    return program.read_operand_strict(op, ValueKind::String);
}

void ensure_same_vector_size(Program& program,
                             const Value& a,
                             const Value& b,
                             const Instruction& ins,
                             const std::string& operation_name) {
    if (a.vec.size() != b.vec.size()) {
        throw_exec_error(program,
                         operation_name + " requires vectors of equal length, got " +
                             std::to_string(a.vec.size()) + " and " + std::to_string(b.vec.size()),
                         &ins);
    }
}

double determinant_recursive(const std::vector<double>& data, size_t n) {
    if (n == 1) {
        return data[0];
    }

    if (n == 2) {
        return data[0] * data[3] - data[1] * data[2];
    }

    double det = 0.0;
    for (size_t col = 0; col < n; col++) {
        std::vector<double> minor;
        minor.reserve((n - 1) * (n - 1));
        for (size_t r = 1; r < n; r++) {
            for (size_t c = 0; c < n; c++) {
                if (c == col) {
                    continue;
                }
                minor.push_back(data[r * n + c]);
            }
        }

        const double sign = (col % 2 == 0) ? 1.0 : -1.0;
        det += sign * data[col] * determinant_recursive(minor, n - 1);
    }
    return det;
}

Value invert_square_matrix(const Value& matrix_value) {
    const size_t n = matrix_value.rows;
    std::vector<double> augmented(n * n * 2, 0.0);
    for (size_t r = 0; r < n; r++) {
        for (size_t c = 0; c < n; c++) {
            augmented[r * (2 * n) + c] = matrix_value.matrix[r * n + c];
            augmented[r * (2 * n) + (n + c)] = (r == c) ? 1.0 : 0.0;
        }
    }

    for (size_t pivot = 0; pivot < n; pivot++) {
        size_t best_row = pivot;
        double best_abs = std::fabs(augmented[pivot * (2 * n) + pivot]);
        for (size_t r = pivot + 1; r < n; r++) {
            double candidate = std::fabs(augmented[r * (2 * n) + pivot]);
            if (candidate > best_abs) {
                best_abs = candidate;
                best_row = r;
            }
        }

        if (best_abs == 0.0) {
            Value null_out{};
            null_out.kind = ValueKind::Null;
            return null_out;
        }

        if (best_row != pivot) {
            for (size_t c = 0; c < 2 * n; c++) {
                std::swap(augmented[pivot * (2 * n) + c], augmented[best_row * (2 * n) + c]);
            }
        }

        double pivot_value = augmented[pivot * (2 * n) + pivot];
        for (size_t c = 0; c < 2 * n; c++) {
            augmented[pivot * (2 * n) + c] /= pivot_value;
        }

        for (size_t r = 0; r < n; r++) {
            if (r == pivot) {
                continue;
            }

            double factor = augmented[r * (2 * n) + pivot];
            for (size_t c = 0; c < 2 * n; c++) {
                augmented[r * (2 * n) + c] -= factor * augmented[pivot * (2 * n) + c];
            }
        }
    }

    Value out{};
    out.kind = ValueKind::Matrix;
    out.rows = n;
    out.cols = n;
    out.matrix.resize(n * n);
    for (size_t r = 0; r < n; r++) {
        for (size_t c = 0; c < n; c++) {
            out.matrix[r * n + c] = augmented[r * (2 * n) + (n + c)];
        }
    }
    return out;
}
} // namespace


std::mt19937 aic::Program::rng(std::random_device{}());

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
    case OperationKind::LIST_NEW: {
        size_t initial_size = read_non_negative_integer_operand(*this, y, ins, "LIST_NEW size");

        Value out{};
        out.kind = ValueKind::List;
        out.list.resize(initial_size);
        for (auto& item : out.list) {
            item.kind = ValueKind::Null;
        }

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::LIST_PUSH: {
        Value list_value = read_operand_strict(x, ValueKind::List);
        Value element = read_operand(y);
        list_value.list.push_back(element);
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::LIST_GET: {
        Value list_value = read_operand_strict(y, ValueKind::List);
        size_t index = read_non_negative_integer_operand(*this, z, ins, "LIST_GET index");
        if (index >= list_value.list.size()) {
            throw_exec_error(*this,
                             "LIST_GET index out of bounds: " + std::to_string(index) +
                                 " for list size " + std::to_string(list_value.list.size()),
                             &ins);
        }

        write_operand(x, list_value.list[index]);
        pc++;
        return;
    }
    case OperationKind::LIST_SET: {
        Value list_value = read_operand_strict(x, ValueKind::List);
        size_t index = read_non_negative_integer_operand(*this, y, ins, "LIST_SET index");
        if (index >= list_value.list.size()) {
            throw_exec_error(*this,
                             "LIST_SET index out of bounds: " + std::to_string(index) +
                                 " for list size " + std::to_string(list_value.list.size()),
                             &ins);
        }

        list_value.list[index] = read_operand(z);
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::LIST_LEN: {
        Value list_value = read_operand_strict(y, ValueKind::List);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = static_cast<int64_t>(list_value.list.size());
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::INT_LIST_NEW: {
        size_t initial_size = read_non_negative_integer_operand(*this, y, ins, "INT_LIST_NEW size");
        Value out{};
        out.kind = ValueKind::IntegerList;
        out.int_list.assign(initial_size, 0);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::INT_LIST_PUSH: {
        Value list_value = read_operand_strict(x, ValueKind::IntegerList);
        Value element = read_operand_strict(y, ValueKind::Integer);
        list_value.int_list.push_back(element.i);
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::INT_LIST_GET: {
        Value list_value = read_operand_strict(y, ValueKind::IntegerList);
        size_t index = read_non_negative_integer_operand(*this, z, ins, "INT_LIST_GET index");
        if (index >= list_value.int_list.size()) {
            throw_exec_error(*this,
                             "INT_LIST_GET index out of bounds: " + std::to_string(index) +
                                 " for list size " + std::to_string(list_value.int_list.size()),
                             &ins);
        }

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = list_value.int_list[index];
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::INT_LIST_SET: {
        Value list_value = read_operand_strict(x, ValueKind::IntegerList);
        size_t index = read_non_negative_integer_operand(*this, y, ins, "INT_LIST_SET index");
        if (index >= list_value.int_list.size()) {
            throw_exec_error(*this,
                             "INT_LIST_SET index out of bounds: " + std::to_string(index) +
                                 " for list size " + std::to_string(list_value.int_list.size()),
                             &ins);
        }

        Value element = read_operand_strict(z, ValueKind::Integer);
        list_value.int_list[index] = element.i;
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::INT_LIST_LEN: {
        Value list_value = read_operand_strict(y, ValueKind::IntegerList);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = static_cast<int64_t>(list_value.int_list.size());
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::FLOAT_LIST_NEW: {
        size_t initial_size = read_non_negative_integer_operand(*this, y, ins, "FLOAT_LIST_NEW size");
        Value out{};
        out.kind = ValueKind::FloatList;
        out.float_list.assign(initial_size, 0.0);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::FLOAT_LIST_PUSH: {
        Value list_value = read_operand_strict(x, ValueKind::FloatList);
        list_value.float_list.push_back(numeric_value_as_double(*this, y, ins, "FLOAT_LIST_PUSH element"));
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::FLOAT_LIST_GET: {
        Value list_value = read_operand_strict(y, ValueKind::FloatList);
        size_t index = read_non_negative_integer_operand(*this, z, ins, "FLOAT_LIST_GET index");
        if (index >= list_value.float_list.size()) {
            throw_exec_error(*this,
                             "FLOAT_LIST_GET index out of bounds: " + std::to_string(index) +
                                 " for list size " + std::to_string(list_value.float_list.size()),
                             &ins);
        }

        Value out{};
        out.kind = ValueKind::Float;
        out.f = list_value.float_list[index];
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::FLOAT_LIST_SET: {
        Value list_value = read_operand_strict(x, ValueKind::FloatList);
        size_t index = read_non_negative_integer_operand(*this, y, ins, "FLOAT_LIST_SET index");
        if (index >= list_value.float_list.size()) {
            throw_exec_error(*this,
                             "FLOAT_LIST_SET index out of bounds: " + std::to_string(index) +
                                 " for list size " + std::to_string(list_value.float_list.size()),
                             &ins);
        }

        list_value.float_list[index] = numeric_value_as_double(*this, z, ins, "FLOAT_LIST_SET value");
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::FLOAT_LIST_LEN: {
        Value list_value = read_operand_strict(y, ValueKind::FloatList);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = static_cast<int64_t>(list_value.float_list.size());
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_NEW: {
        size_t initial_size = read_non_negative_integer_operand(*this, y, ins, "VEC_NEW size");
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.assign(initial_size, 0.0);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_PUSH: {
        Value vec_value = read_vector_strict(*this, x);
        vec_value.vec.push_back(numeric_value_as_double(*this, y, ins, "VEC_PUSH element"));
        write_operand(x, vec_value);
        pc++;
        return;
    }
    case OperationKind::VEC_GET: {
        Value vec_value = read_vector_strict(*this, y);
        size_t index = read_non_negative_integer_operand(*this, z, ins, "VEC_GET index");
        if (index >= vec_value.vec.size()) {
            throw_exec_error(*this,
                             "VEC_GET index out of bounds: " + std::to_string(index) +
                                 " for vector size " + std::to_string(vec_value.vec.size()),
                             &ins);
        }
        Value out{};
        out.kind = ValueKind::Float;
        out.f = vec_value.vec[index];
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_SET: {
        Value vec_value = read_vector_strict(*this, x);
        size_t index = read_non_negative_integer_operand(*this, y, ins, "VEC_SET index");
        if (index >= vec_value.vec.size()) {
            throw_exec_error(*this,
                             "VEC_SET index out of bounds: " + std::to_string(index) +
                                 " for vector size " + std::to_string(vec_value.vec.size()),
                             &ins);
        }
        vec_value.vec[index] = numeric_value_as_double(*this, z, ins, "VEC_SET value");
        write_operand(x, vec_value);
        pc++;
        return;
    }
    case OperationKind::VEC_LEN: {
        Value vec_value = read_vector_strict(*this, y);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = static_cast<int64_t>(vec_value.vec.size());
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_ADD: {
        Value lhs = read_vector_strict(*this, y);
        Value rhs = read_vector_strict(*this, z);
        ensure_same_vector_size(*this, lhs, rhs, ins, "VEC_ADD");
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(lhs.vec.size());
        for (size_t i = 0; i < lhs.vec.size(); i++) {
            out.vec[i] = lhs.vec[i] + rhs.vec[i];
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_SUB: {
        Value lhs = read_vector_strict(*this, y);
        Value rhs = read_vector_strict(*this, z);
        ensure_same_vector_size(*this, lhs, rhs, ins, "VEC_SUB");
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(lhs.vec.size());
        for (size_t i = 0; i < lhs.vec.size(); i++) {
            out.vec[i] = lhs.vec[i] - rhs.vec[i];
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_SCALE: {
        Value lhs = read_vector_strict(*this, y);
        double scale = numeric_value_as_double(*this, z, ins, "VEC_SCALE scale");
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(lhs.vec.size());
        for (size_t i = 0; i < lhs.vec.size(); i++) {
            out.vec[i] = lhs.vec[i] * scale;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_DOT: {
        Value lhs = read_vector_strict(*this, y);
        Value rhs = read_vector_strict(*this, z);
        ensure_same_vector_size(*this, lhs, rhs, ins, "VEC_DOT");
        double dot = 0.0;
        for (size_t i = 0; i < lhs.vec.size(); i++) {
            dot += lhs.vec[i] * rhs.vec[i];
        }
        Value out{};
        out.kind = ValueKind::Float;
        out.f = dot;
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_CROSS: {
        Value lhs = read_vector_strict(*this, y);
        Value rhs = read_vector_strict(*this, z);
        if (lhs.vec.size() != 3 || rhs.vec.size() != 3) {
            throw_exec_error(*this, "VEC_CROSS requires two 3D vectors", &ins);
        }
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec = {
            lhs.vec[1] * rhs.vec[2] - lhs.vec[2] * rhs.vec[1],
            lhs.vec[2] * rhs.vec[0] - lhs.vec[0] * rhs.vec[2],
            lhs.vec[0] * rhs.vec[1] - lhs.vec[1] * rhs.vec[0]
        };
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_MAG: {
        Value vec_value = read_vector_strict(*this, y);
        double sum = 0.0;
        for (double value : vec_value.vec) {
            sum += value * value;
        }
        Value out{};
        out.kind = ValueKind::Float;
        out.f = std::sqrt(sum);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_NORM: {
        Value vec_value = read_vector_strict(*this, y);
        double sum = 0.0;
        for (double value : vec_value.vec) {
            sum += value * value;
        }
        double mag = std::sqrt(sum);
        if (mag == 0.0) {
            throw_exec_error(*this, "VEC_NORM cannot normalize a zero vector", &ins);
        }
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(vec_value.vec.size());
        for (size_t i = 0; i < vec_value.vec.size(); i++) {
            out.vec[i] = vec_value.vec[i] / mag;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_NEW: {
        size_t rows = read_non_negative_integer_operand(*this, y, ins, "MAT_NEW rows");
        size_t cols = read_non_negative_integer_operand(*this, z, ins, "MAT_NEW cols");
        Value out{};
        out.kind = ValueKind::Matrix;
        out.rows = rows;
        out.cols = cols;
        out.matrix.assign(rows * cols, 0.0);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_GET: {
        Value matrix_value = read_matrix_strict(*this, y);
        size_t index = read_non_negative_integer_operand(*this, z, ins, "MAT_GET index");
        if (index >= matrix_value.matrix.size()) {
            throw_exec_error(*this,
                             "MAT_GET index out of bounds: " + std::to_string(index) +
                                 " for matrix size " + std::to_string(matrix_value.matrix.size()),
                             &ins);
        }
        Value out{};
        out.kind = ValueKind::Float;
        out.f = matrix_value.matrix[index];
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_SET: {
        Value matrix_value = read_matrix_strict(*this, x);
        size_t index = read_non_negative_integer_operand(*this, y, ins, "MAT_SET index");
        if (index >= matrix_value.matrix.size()) {
            throw_exec_error(*this,
                             "MAT_SET index out of bounds: " + std::to_string(index) +
                                 " for matrix size " + std::to_string(matrix_value.matrix.size()),
                             &ins);
        }
        matrix_value.matrix[index] = numeric_value_as_double(*this, z, ins, "MAT_SET value");
        write_operand(x, matrix_value);
        pc++;
        return;
    }
    case OperationKind::MAT_ROWS: {
        Value matrix_value = read_matrix_strict(*this, y);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = static_cast<int64_t>(matrix_value.rows);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_COLS: {
        Value matrix_value = read_matrix_strict(*this, y);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = static_cast<int64_t>(matrix_value.cols);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_MUL: {
        Value a = read_matrix_strict(*this, y);
        Value b = read_matrix_strict(*this, z);
        if (a.cols != b.rows) {
            throw_exec_error(*this,
                             "MAT_MUL dimension mismatch: lhs is " + std::to_string(a.rows) +
                                 "x" + std::to_string(a.cols) + ", rhs is " +
                                 std::to_string(b.rows) + "x" + std::to_string(b.cols),
                             &ins);
        }

        Value out{};
        out.kind = ValueKind::Matrix;
        out.rows = a.rows;
        out.cols = b.cols;
        out.matrix.assign(out.rows * out.cols, 0.0);
        for (size_t r = 0; r < out.rows; r++) {
            for (size_t c = 0; c < out.cols; c++) {
                double sum = 0.0;
                for (size_t k = 0; k < a.cols; k++) {
                    sum += a.matrix[r * a.cols + k] * b.matrix[k * b.cols + c];
                }
                out.matrix[r * out.cols + c] = sum;
            }
        }

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_TRANSPOSE: {
        Value in = read_matrix_strict(*this, y);
        Value out{};
        out.kind = ValueKind::Matrix;
        out.rows = in.cols;
        out.cols = in.rows;
        out.matrix.assign(out.rows * out.cols, 0.0);
        for (size_t r = 0; r < in.rows; r++) {
            for (size_t c = 0; c < in.cols; c++) {
                out.matrix[c * out.cols + r] = in.matrix[r * in.cols + c];
            }
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_DET: {
        Value in = read_matrix_strict(*this, y);
        if (in.rows != in.cols) {
            throw_exec_error(*this, "MAT_DET requires a square matrix", &ins);
        }

        Value out{};
        out.kind = ValueKind::Float;
        out.f = determinant_recursive(in.matrix, in.rows);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_INV: {
        Value in = read_matrix_strict(*this, y);
        if (in.rows != in.cols) {
            throw_exec_error(*this, "MAT_INV requires a square matrix", &ins);
        }

        Value out = invert_square_matrix(in);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_CONCAT: {
        Value lhs = read_string_strict(*this, y);
        Value rhs = read_string_strict(*this, z);

        Value out{};
        out.kind = ValueKind::String;
        out.s = lhs.s + rhs.s;

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_LEN: {
        Value in = read_string_strict(*this, y);

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = in.s.length();

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::RAND_RANDOM: {
        Value out{};
        out.kind = ValueKind::Float;
        out.f = random_decimal(*this);

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::RAND_INT: {
        Value minv = read_operand(y);
        Value maxv = read_operand(z);

        if (minv.kind != ValueKind::Integer || maxv.kind != ValueKind::Integer) {
            throw_exec_error(*this, "RAND_INT requires integer min and max");
        }

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = random_int(*this, minv.i, maxv.i);

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::RAND_RANGE: {
        Value minv = read_operand(y);
        Value maxv = read_operand(z);

        float minf;
        float maxf;

        if (minv.kind == ValueKind::Integer) minf = static_cast<float>(minv.i);
        else if (minv.kind == ValueKind::Float) minf = minv.f;
        else throw_exec_error(*this, "RAND_RANGE requires numeric min");

        if (maxv.kind == ValueKind::Integer) maxf = static_cast<float>(maxv.i);
        else if (maxv.kind == ValueKind::Float) maxf = maxv.f;
        else throw_exec_error(*this, "RAND_RANGE requires numeric max");

        Value out{};
        out.kind = ValueKind::Float;
        out.f = random_range(*this, minf, maxf);

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
