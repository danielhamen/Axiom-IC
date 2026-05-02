#include "program.hpp"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numbers>
#include <numeric>
#include <random>
#include <stdexcept>
#include <thread>
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

Value read_numeric_value(Program& program, const Operand& op, const Instruction& ins, const std::string& usage) {
    Value v = program.read_operand(op);
    if (v.kind == ValueKind::Integer || v.kind == ValueKind::Float) {
        return v;
    }

    throw_exec_error(program,
                     usage + " expects Integer or Float, got " + value_kind_to_string(v.kind),
                     &ins);
}

bool value_equals(const Value& lhs, const Value& rhs) {
    if ((lhs.kind == ValueKind::Integer || lhs.kind == ValueKind::Float) &&
        (rhs.kind == ValueKind::Integer || rhs.kind == ValueKind::Float)) {
        const double left_num = lhs.kind == ValueKind::Integer ? static_cast<double>(lhs.i) : lhs.f;
        const double right_num = rhs.kind == ValueKind::Integer ? static_cast<double>(rhs.i) : rhs.f;
        return left_num == right_num;
    }

    if (lhs.kind != rhs.kind) {
        return false;
    }

    switch (lhs.kind) {
        case ValueKind::Boolean:
            return lhs.b == rhs.b;
        case ValueKind::String:
            return lhs.s == rhs.s;
        case ValueKind::Null:
            return true;
        default:
            return lhs.to_str() == rhs.to_str();
    }
}

Value make_bool_value(bool b) {
    Value out{};
    out.kind = ValueKind::Boolean;
    out.b = b;
    return out;
}

Value make_numeric_value(double v) {
    Value out{};
    out.kind = ValueKind::Float;
    out.f = v;
    return out;
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

double vector_magnitude(const Value& vec_value) {
    double sum = 0.0;
    for (double value : vec_value.vec) {
        sum += value * value;
    }
    return std::sqrt(sum);
}

std::vector<double> matrix_rref(std::vector<double> data, size_t rows, size_t cols, size_t& rank_out) {
    const double eps = 1e-12;
    size_t r = 0;
    rank_out = 0;
    for (size_t c = 0; c < cols && r < rows; c++) {
        size_t pivot = r;
        double best_abs = std::fabs(data[pivot * cols + c]);
        for (size_t i = r + 1; i < rows; i++) {
            const double candidate = std::fabs(data[i * cols + c]);
            if (candidate > best_abs) {
                best_abs = candidate;
                pivot = i;
            }
        }
        if (best_abs <= eps) {
            continue;
        }
        if (pivot != r) {
            for (size_t j = 0; j < cols; j++) {
                std::swap(data[r * cols + j], data[pivot * cols + j]);
            }
        }
        const double pivot_value = data[r * cols + c];
        for (size_t j = 0; j < cols; j++) {
            data[r * cols + j] /= pivot_value;
        }
        for (size_t i = 0; i < rows; i++) {
            if (i == r) {
                continue;
            }
            const double factor = data[i * cols + c];
            if (std::fabs(factor) <= eps) {
                continue;
            }
            for (size_t j = 0; j < cols; j++) {
                data[i * cols + j] -= factor * data[r * cols + j];
            }
        }
        r++;
        rank_out++;
    }
    return data;
}

std::string trim_copy(const std::string& in) {
    size_t left = 0;
    while (left < in.size() && std::isspace(static_cast<unsigned char>(in[left]))) {
        left++;
    }
    size_t right = in.size();
    while (right > left && std::isspace(static_cast<unsigned char>(in[right - 1]))) {
        right--;
    }
    return in.substr(left, right - left);
}

void jump_to_label(Program& program, const Operand& label_operand, const Instruction& ins, const std::string& opname) {
    if (label_operand.kind != OperandKind::Label) {
        throw_exec_error(program,
                         opname + " expects Label destination as first operand, got " + label_operand.kindstr(),
                         &ins);
    }
    Function* current_function = program.functions.at(program.fc);
    const auto& labels = current_function->labels;
    const std::string& label_name = label_operand.strval;
    if (!labels.contains(label_name)) {
        throw_exec_error(program, opname + " references undefined label '" + label_name + "'", &ins);
    }
    const size_t dst_pc = labels.at(label_name);
    if (dst_pc >= current_function->ins.size()) {
        throw_exec_error(program, opname + " target label pc out of range", &ins);
    }
    program.pc = dst_pc;
}

bool value_truthy(const Value& value) {
    switch (value.kind) {
        case ValueKind::Boolean:
            return value.b;
        case ValueKind::Integer:
            return value.i != 0;
        case ValueKind::Float:
            return value.f != 0.0;
        case ValueKind::String:
            return !value.s.empty();
        case ValueKind::Null:
            return false;
        case ValueKind::List:
            return !value.list.empty();
        case ValueKind::IntegerList:
            return !value.int_list.empty();
        case ValueKind::FloatList:
            return !value.float_list.empty();
        case ValueKind::Vector:
            return !value.vec.empty();
        case ValueKind::Matrix:
            return !value.matrix.empty();
    }
    return false;
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
        Value lhs = read_numeric_value(*this, y, ins, "ADD");
        Value rhs = read_numeric_value(*this, z, ins, "ADD");

        Value out{};
        if (lhs.kind == ValueKind::Integer && rhs.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = lhs.i + rhs.i;
        } else {
            out.kind = ValueKind::Float;
            out.f = (lhs.kind == ValueKind::Integer ? static_cast<double>(lhs.i) : lhs.f) +
                    (rhs.kind == ValueKind::Integer ? static_cast<double>(rhs.i) : rhs.f);
        }

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MUL: {
        Value lhs = read_numeric_value(*this, y, ins, "MUL");
        Value rhs = read_numeric_value(*this, z, ins, "MUL");

        Value out{};
        if (lhs.kind == ValueKind::Integer && rhs.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = lhs.i * rhs.i;
        } else {
            out.kind = ValueKind::Float;
            out.f = (lhs.kind == ValueKind::Integer ? static_cast<double>(lhs.i) : lhs.f) *
                    (rhs.kind == ValueKind::Integer ? static_cast<double>(rhs.i) : rhs.f);
        }

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::SUB: {
        Value lhs = read_numeric_value(*this, y, ins, "SUB");
        Value rhs = read_numeric_value(*this, z, ins, "SUB");

        Value out{};
        if (lhs.kind == ValueKind::Integer && rhs.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = lhs.i - rhs.i;
        } else {
            out.kind = ValueKind::Float;
            out.f = (lhs.kind == ValueKind::Integer ? static_cast<double>(lhs.i) : lhs.f) -
                    (rhs.kind == ValueKind::Integer ? static_cast<double>(rhs.i) : rhs.f);
        }

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::DIV: {
        Value lhs = read_numeric_value(*this, y, ins, "DIV");
        Value rhs = read_numeric_value(*this, z, ins, "DIV");
        const double rhs_num = rhs.kind == ValueKind::Integer ? static_cast<double>(rhs.i) : rhs.f;
        if (rhs_num == 0.0) {
            throw_exec_error(*this, "Division by zero", &ins);
        }

        Value out{};
        if (lhs.kind == ValueKind::Integer && rhs.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = lhs.i / rhs.i;
        } else {
            out.kind = ValueKind::Float;
            out.f = (lhs.kind == ValueKind::Integer ? static_cast<double>(lhs.i) : lhs.f) / rhs_num;
        }

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MOD: {
        Value lhs = read_operand_strict(y, ValueKind::Integer);
        Value rhs = read_operand_strict(z, ValueKind::Integer);

        if (rhs.i == 0) {
            throw_exec_error(*this, "Modulo by zero", &ins);
        }

        Value out{};
        out.kind = ValueKind::Integer;
        out.i = lhs.i % rhs.i;

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::NEG: {
        Value v = read_numeric_value(*this, y, ins, "NEG");
        Value out{};
        if (v.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = -v.i;
        } else {
            out.kind = ValueKind::Float;
            out.f = -v.f;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::INC: {
        Value v = read_numeric_value(*this, y, ins, "INC");
        Value out{};
        if (v.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = v.i + 1;
        } else {
            out.kind = ValueKind::Float;
            out.f = v.f + 1.0;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::DEC: {
        Value v = read_numeric_value(*this, y, ins, "DEC");
        Value out{};
        if (v.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = v.i - 1;
        } else {
            out.kind = ValueKind::Float;
            out.f = v.f - 1.0;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::ABS: {
        Value v = read_numeric_value(*this, y, ins, "ABS");
        Value out{};
        if (v.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = std::abs(v.i);
        } else {
            out.kind = ValueKind::Float;
            out.f = std::fabs(v.f);
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::EQ: {
        write_operand(x, make_bool_value(value_equals(read_operand(y), read_operand(z))));
        pc++;
        return;
    }
    case OperationKind::NEQ: {
        write_operand(x, make_bool_value(!value_equals(read_operand(y), read_operand(z))));
        pc++;
        return;
    }
    case OperationKind::LT: {
        write_operand(x, make_bool_value(numeric_value_as_double(*this, y, ins, "LT") <
                                         numeric_value_as_double(*this, z, ins, "LT")));
        pc++;
        return;
    }
    case OperationKind::LTE: {
        write_operand(x, make_bool_value(numeric_value_as_double(*this, y, ins, "LTE") <=
                                         numeric_value_as_double(*this, z, ins, "LTE")));
        pc++;
        return;
    }
    case OperationKind::GT: {
        write_operand(x, make_bool_value(numeric_value_as_double(*this, y, ins, "GT") >
                                         numeric_value_as_double(*this, z, ins, "GT")));
        pc++;
        return;
    }
    case OperationKind::GTE: {
        write_operand(x, make_bool_value(numeric_value_as_double(*this, y, ins, "GTE") >=
                                         numeric_value_as_double(*this, z, ins, "GTE")));
        pc++;
        return;
    }
    case OperationKind::AND: {
        Value lhs = read_operand(y);
        Value rhs = read_operand(z);
        if (lhs.kind == ValueKind::Boolean && rhs.kind == ValueKind::Boolean) {
            write_operand(x, make_bool_value(lhs.b && rhs.b));
        } else if (lhs.kind == ValueKind::Integer && rhs.kind == ValueKind::Integer) {
            Value out{};
            out.kind = ValueKind::Integer;
            out.i = lhs.i & rhs.i;
            write_operand(x, out);
        } else {
            throw_exec_error(*this, "AND expects (Boolean,Boolean) or (Integer,Integer)", &ins);
        }
        pc++;
        return;
    }
    case OperationKind::OR: {
        Value lhs = read_operand(y);
        Value rhs = read_operand(z);
        if (lhs.kind == ValueKind::Boolean && rhs.kind == ValueKind::Boolean) {
            write_operand(x, make_bool_value(lhs.b || rhs.b));
        } else if (lhs.kind == ValueKind::Integer && rhs.kind == ValueKind::Integer) {
            Value out{};
            out.kind = ValueKind::Integer;
            out.i = lhs.i | rhs.i;
            write_operand(x, out);
        } else {
            throw_exec_error(*this, "OR expects (Boolean,Boolean) or (Integer,Integer)", &ins);
        }
        pc++;
        return;
    }
    case OperationKind::NOT: {
        Value v = read_operand(y);
        if (v.kind == ValueKind::Boolean) {
            write_operand(x, make_bool_value(!v.b));
        } else if (v.kind == ValueKind::Integer) {
            Value out{};
            out.kind = ValueKind::Integer;
            out.i = ~v.i;
            write_operand(x, out);
        } else {
            throw_exec_error(*this, "NOT expects Boolean or Integer", &ins);
        }
        pc++;
        return;
    }
    case OperationKind::XOR: {
        Value lhs = read_operand(y);
        Value rhs = read_operand(z);
        if (lhs.kind == ValueKind::Boolean && rhs.kind == ValueKind::Boolean) {
            write_operand(x, make_bool_value(lhs.b != rhs.b));
        } else if (lhs.kind == ValueKind::Integer && rhs.kind == ValueKind::Integer) {
            Value out{};
            out.kind = ValueKind::Integer;
            out.i = lhs.i ^ rhs.i;
            write_operand(x, out);
        } else {
            throw_exec_error(*this, "XOR expects (Boolean,Boolean) or (Integer,Integer)", &ins);
        }
        pc++;
        return;
    }
    case OperationKind::POW: {
        write_operand(x, make_numeric_value(std::pow(numeric_value_as_double(*this, y, ins, "POW"),
                                                     numeric_value_as_double(*this, z, ins, "POW"))));
        pc++;
        return;
    }
    case OperationKind::SQRT: {
        write_operand(x, make_numeric_value(std::sqrt(numeric_value_as_double(*this, y, ins, "SQRT"))));
        pc++;
        return;
    }
    case OperationKind::EXP: {
        write_operand(x, make_numeric_value(std::exp(numeric_value_as_double(*this, y, ins, "EXP"))));
        pc++;
        return;
    }
    case OperationKind::LOG: {
        const double value = numeric_value_as_double(*this, y, ins, "LOG");
        const double base = numeric_value_as_double(*this, z, ins, "LOG");
        write_operand(x, make_numeric_value(std::log(value) / std::log(base)));
        pc++;
        return;
    }
    case OperationKind::LOG2: {
        write_operand(x, make_numeric_value(std::log2(numeric_value_as_double(*this, y, ins, "LOG2"))));
        pc++;
        return;
    }
    case OperationKind::LOG10: {
        write_operand(x, make_numeric_value(std::log10(numeric_value_as_double(*this, y, ins, "LOG10"))));
        pc++;
        return;
    }
    case OperationKind::LN: {
        write_operand(x, make_numeric_value(std::log(numeric_value_as_double(*this, y, ins, "LN"))));
        pc++;
        return;
    }
    case OperationKind::SIN: { write_operand(x, make_numeric_value(std::sin(numeric_value_as_double(*this, y, ins, "SIN")))); pc++; return; }
    case OperationKind::COS: { write_operand(x, make_numeric_value(std::cos(numeric_value_as_double(*this, y, ins, "COS")))); pc++; return; }
    case OperationKind::TAN: { write_operand(x, make_numeric_value(std::tan(numeric_value_as_double(*this, y, ins, "TAN")))); pc++; return; }
    case OperationKind::CSC: { write_operand(x, make_numeric_value(1.0 / std::sin(numeric_value_as_double(*this, y, ins, "CSC")))); pc++; return; }
    case OperationKind::SEC: { write_operand(x, make_numeric_value(1.0 / std::cos(numeric_value_as_double(*this, y, ins, "SEC")))); pc++; return; }
    case OperationKind::COT: { write_operand(x, make_numeric_value(1.0 / std::tan(numeric_value_as_double(*this, y, ins, "COT")))); pc++; return; }
    case OperationKind::ASIN: { write_operand(x, make_numeric_value(std::asin(numeric_value_as_double(*this, y, ins, "ASIN")))); pc++; return; }
    case OperationKind::ACOS: { write_operand(x, make_numeric_value(std::acos(numeric_value_as_double(*this, y, ins, "ACOS")))); pc++; return; }
    case OperationKind::ATAN: { write_operand(x, make_numeric_value(std::atan(numeric_value_as_double(*this, y, ins, "ATAN")))); pc++; return; }
    case OperationKind::ACSC: { write_operand(x, make_numeric_value(std::asin(1.0 / numeric_value_as_double(*this, y, ins, "ACSC")))); pc++; return; }
    case OperationKind::ASEC: { write_operand(x, make_numeric_value(std::acos(1.0 / numeric_value_as_double(*this, y, ins, "ASEC")))); pc++; return; }
    case OperationKind::ACOT: { write_operand(x, make_numeric_value(std::atan(1.0 / numeric_value_as_double(*this, y, ins, "ACOT")))); pc++; return; }
    case OperationKind::SINH: { write_operand(x, make_numeric_value(std::sinh(numeric_value_as_double(*this, y, ins, "SINH")))); pc++; return; }
    case OperationKind::COSH: { write_operand(x, make_numeric_value(std::cosh(numeric_value_as_double(*this, y, ins, "COSH")))); pc++; return; }
    case OperationKind::TANH: { write_operand(x, make_numeric_value(std::tanh(numeric_value_as_double(*this, y, ins, "TANH")))); pc++; return; }
    case OperationKind::CSCH: { write_operand(x, make_numeric_value(1.0 / std::sinh(numeric_value_as_double(*this, y, ins, "CSCH")))); pc++; return; }
    case OperationKind::SECH: { write_operand(x, make_numeric_value(1.0 / std::cosh(numeric_value_as_double(*this, y, ins, "SECH")))); pc++; return; }
    case OperationKind::COTH: { write_operand(x, make_numeric_value(1.0 / std::tanh(numeric_value_as_double(*this, y, ins, "COTH")))); pc++; return; }
    case OperationKind::ASINH: { write_operand(x, make_numeric_value(std::asinh(numeric_value_as_double(*this, y, ins, "ASINH")))); pc++; return; }
    case OperationKind::ACOSH: { write_operand(x, make_numeric_value(std::acosh(numeric_value_as_double(*this, y, ins, "ACOSH")))); pc++; return; }
    case OperationKind::ATANH: { write_operand(x, make_numeric_value(std::atanh(numeric_value_as_double(*this, y, ins, "ATANH")))); pc++; return; }
    case OperationKind::ACSCH: { write_operand(x, make_numeric_value(std::asinh(1.0 / numeric_value_as_double(*this, y, ins, "ACSCH")))); pc++; return; }
    case OperationKind::ASECH: { write_operand(x, make_numeric_value(std::acosh(1.0 / numeric_value_as_double(*this, y, ins, "ASECH")))); pc++; return; }
    case OperationKind::ACOTH: { write_operand(x, make_numeric_value(std::atanh(1.0 / numeric_value_as_double(*this, y, ins, "ACOTH")))); pc++; return; }
    case OperationKind::ATAN2: { write_operand(x, make_numeric_value(std::atan2(numeric_value_as_double(*this, y, ins, "ATAN2"), numeric_value_as_double(*this, z, ins, "ATAN2")))); pc++; return; }
    case OperationKind::HYPOT: { write_operand(x, make_numeric_value(std::hypot(numeric_value_as_double(*this, y, ins, "HYPOT"), numeric_value_as_double(*this, z, ins, "HYPOT")))); pc++; return; }
    case OperationKind::FLOOR: { write_operand(x, make_numeric_value(std::floor(numeric_value_as_double(*this, y, ins, "FLOOR")))); pc++; return; }
    case OperationKind::CEIL: { write_operand(x, make_numeric_value(std::ceil(numeric_value_as_double(*this, y, ins, "CEIL")))); pc++; return; }
    case OperationKind::ROUND: { write_operand(x, make_numeric_value(std::round(numeric_value_as_double(*this, y, ins, "ROUND")))); pc++; return; }
    case OperationKind::MIN: { write_operand(x, make_numeric_value(std::fmin(numeric_value_as_double(*this, y, ins, "MIN"), numeric_value_as_double(*this, z, ins, "MIN")))); pc++; return; }
    case OperationKind::MAX: { write_operand(x, make_numeric_value(std::fmax(numeric_value_as_double(*this, y, ins, "MAX"), numeric_value_as_double(*this, z, ins, "MAX")))); pc++; return; }
    case OperationKind::CLAMP: {
        const double value = numeric_value_as_double(*this, x, ins, "CLAMP");
        const double lower = numeric_value_as_double(*this, y, ins, "CLAMP");
        const double upper = numeric_value_as_double(*this, z, ins, "CLAMP");
        write_operand(x, make_numeric_value(std::clamp(value, lower, upper)));
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

        const auto label_it = labels.find(label_name);
        if (label_it == labels.end())
            throw_exec_error(*this,
                             "Undefined label reference '" + label_name + "' in current function scope",
                             &ins);

        size_t dst_pc = label_it->second;
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
    case OperationKind::PRINTLN: {
        if (x.is_none() || !y.is_none() || !z.is_none())
            throw_exec_error(*this,
                             "Operand count mismatch for PRINTLN: expected exactly one operand",
                             &ins);

        Value v = read_operand(x);
        std::cout << v.to_str() << std::endl;

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

        const auto callee_index = functions.try_index_of(x.strval);
        if (!callee_index.has_value()) {
            throw_exec_error(*this, "CALL target function does not exist: " + x.strval, &ins);
        }

        size_t callee_fc = *callee_index;
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
    case OperationKind::LIST_POP: {
        Value list_value = read_operand_strict(y, ValueKind::List);
        if (list_value.list.empty()) {
            throw_exec_error(*this, "LIST_POP attempted on an empty list", &ins);
        }
        Value popped = list_value.list.back();
        list_value.list.pop_back();
        write_operand(y, list_value);
        write_operand(x, popped);
        pc++;
        return;
    }
    case OperationKind::LIST_INSERT: {
        Value list_value = read_operand_strict(x, ValueKind::List);
        const size_t index = read_non_negative_integer_operand(*this, y, ins, "LIST_INSERT index");
        if (index > list_value.list.size()) {
            throw_exec_error(*this, "LIST_INSERT index out of bounds", &ins);
        }
        Value element = read_operand(z);
        list_value.list.insert(list_value.list.begin() + static_cast<std::ptrdiff_t>(index), element);
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::LIST_ERASE: {
        Value list_value = read_operand_strict(x, ValueKind::List);
        const size_t index = read_non_negative_integer_operand(*this, y, ins, "LIST_ERASE index");
        if (index >= list_value.list.size()) {
            throw_exec_error(*this, "LIST_ERASE index out of bounds", &ins);
        }
        list_value.list.erase(list_value.list.begin() + static_cast<std::ptrdiff_t>(index));
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::LIST_SLICE: {
        Value list_value = read_operand_strict(y, ValueKind::List);
        const size_t start = read_non_negative_integer_operand(*this, z, ins, "LIST_SLICE start");
        if (start > list_value.list.size()) {
            throw_exec_error(*this, "LIST_SLICE start out of bounds", &ins);
        }
        Value out{};
        out.kind = ValueKind::List;
        out.list.assign(list_value.list.begin() + static_cast<std::ptrdiff_t>(start), list_value.list.end());
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::LIST_CLEAR: {
        Value list_value = read_operand_strict(x, ValueKind::List);
        list_value.list.clear();
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::LIST_FIND: {
        Value list_value = read_operand_strict(y, ValueKind::List);
        Value needle = read_operand(z);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = -1;
        for (size_t i = 0; i < list_value.list.size(); i++) {
            if (value_equals(list_value.list[i], needle)) {
                out.i = static_cast<int64_t>(i);
                break;
            }
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::LIST_SORT: {
        Value list_value = read_operand_strict(x, ValueKind::List);
        std::sort(list_value.list.begin(),
                  list_value.list.end(),
                  [](const Value& a, const Value& b) { return a.to_str() < b.to_str(); });
        write_operand(x, list_value);
        pc++;
        return;
    }
    case OperationKind::LIST_REVERSE: {
        Value list_value = read_operand_strict(x, ValueKind::List);
        std::reverse(list_value.list.begin(), list_value.list.end());
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
        double mag = vector_magnitude(vec_value);
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
    case OperationKind::VEC_PROJECT: {
        Value a = read_vector_strict(*this, y);
        Value b = read_vector_strict(*this, z);
        ensure_same_vector_size(*this, a, b, ins, "VEC_PROJECT");
        double bb = 0.0;
        double ab = 0.0;
        for (size_t i = 0; i < a.vec.size(); i++) {
            ab += a.vec[i] * b.vec[i];
            bb += b.vec[i] * b.vec[i];
        }
        if (bb == 0.0) {
            throw_exec_error(*this, "VEC_PROJECT cannot project onto zero vector", &ins);
        }
        const double factor = ab / bb;
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(b.vec.size());
        for (size_t i = 0; i < b.vec.size(); i++) {
            out.vec[i] = b.vec[i] * factor;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_REFLECT: {
        Value v = read_vector_strict(*this, y);
        Value n = read_vector_strict(*this, z);
        ensure_same_vector_size(*this, v, n, ins, "VEC_REFLECT");
        double nn = 0.0;
        double vn = 0.0;
        for (size_t i = 0; i < v.vec.size(); i++) {
            vn += v.vec[i] * n.vec[i];
            nn += n.vec[i] * n.vec[i];
        }
        if (nn == 0.0) {
            throw_exec_error(*this, "VEC_REFLECT normal must be non-zero", &ins);
        }
        const double factor = 2.0 * vn / nn;
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(v.vec.size());
        for (size_t i = 0; i < v.vec.size(); i++) {
            out.vec[i] = v.vec[i] - factor * n.vec[i];
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_ANGLE: {
        Value a = read_vector_strict(*this, y);
        Value b = read_vector_strict(*this, z);
        ensure_same_vector_size(*this, a, b, ins, "VEC_ANGLE");
        const double amag = vector_magnitude(a);
        const double bmag = vector_magnitude(b);
        if (amag == 0.0 || bmag == 0.0) {
            throw_exec_error(*this, "VEC_ANGLE requires non-zero vectors", &ins);
        }
        double dot = 0.0;
        for (size_t i = 0; i < a.vec.size(); i++) {
            dot += a.vec[i] * b.vec[i];
        }
        Value out{};
        out.kind = ValueKind::Float;
        out.f = std::acos(std::clamp(dot / (amag * bmag), -1.0, 1.0));
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_DISTANCE: {
        Value a = read_vector_strict(*this, y);
        Value b = read_vector_strict(*this, z);
        ensure_same_vector_size(*this, a, b, ins, "VEC_DISTANCE");
        double sum = 0.0;
        for (size_t i = 0; i < a.vec.size(); i++) {
            const double d = a.vec[i] - b.vec[i];
            sum += d * d;
        }
        Value out{};
        out.kind = ValueKind::Float;
        out.f = std::sqrt(sum);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::VEC_LERP: {
        Value a = read_vector_strict(*this, y);
        Value b = read_vector_strict(*this, z);
        ensure_same_vector_size(*this, a, b, ins, "VEC_LERP");
        const double t = std::clamp(numeric_value_as_double(*this, x, ins, "VEC_LERP t"), 0.0, 1.0);
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(a.vec.size());
        for (size_t i = 0; i < a.vec.size(); i++) {
            out.vec[i] = a.vec[i] + (b.vec[i] - a.vec[i]) * t;
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
    case OperationKind::MAT_ADD: {
        Value a = read_matrix_strict(*this, y);
        Value b = read_matrix_strict(*this, z);
        if (a.rows != b.rows || a.cols != b.cols) {
            throw_exec_error(*this, "MAT_ADD requires matrices with matching dimensions", &ins);
        }
        Value out{};
        out.kind = ValueKind::Matrix;
        out.rows = a.rows;
        out.cols = a.cols;
        out.matrix.resize(a.matrix.size());
        for (size_t i = 0; i < a.matrix.size(); i++) {
            out.matrix[i] = a.matrix[i] + b.matrix[i];
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_SUB: {
        Value a = read_matrix_strict(*this, y);
        Value b = read_matrix_strict(*this, z);
        if (a.rows != b.rows || a.cols != b.cols) {
            throw_exec_error(*this, "MAT_SUB requires matrices with matching dimensions", &ins);
        }
        Value out{};
        out.kind = ValueKind::Matrix;
        out.rows = a.rows;
        out.cols = a.cols;
        out.matrix.resize(a.matrix.size());
        for (size_t i = 0; i < a.matrix.size(); i++) {
            out.matrix[i] = a.matrix[i] - b.matrix[i];
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_SCALE: {
        Value a = read_matrix_strict(*this, y);
        double s = numeric_value_as_double(*this, z, ins, "MAT_SCALE scale");
        Value out = a;
        for (double& value : out.matrix) {
            value *= s;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_IDENTITY: {
        size_t n = read_non_negative_integer_operand(*this, y, ins, "MAT_IDENTITY size");
        Value out{};
        out.kind = ValueKind::Matrix;
        out.rows = n;
        out.cols = n;
        out.matrix.assign(n * n, 0.0);
        for (size_t i = 0; i < n; i++) {
            out.matrix[i * n + i] = 1.0;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_TRACE: {
        Value m = read_matrix_strict(*this, y);
        if (m.rows != m.cols) {
            throw_exec_error(*this, "MAT_TRACE requires a square matrix", &ins);
        }
        Value out{};
        out.kind = ValueKind::Float;
        out.f = 0.0;
        for (size_t i = 0; i < m.rows; i++) {
            out.f += m.matrix[i * m.cols + i];
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_RREF: {
        Value m = read_matrix_strict(*this, y);
        size_t rank = 0;
        Value out{};
        out.kind = ValueKind::Matrix;
        out.rows = m.rows;
        out.cols = m.cols;
        out.matrix = matrix_rref(m.matrix, m.rows, m.cols, rank);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_RANK: {
        Value m = read_matrix_strict(*this, y);
        size_t rank = 0;
        (void)matrix_rref(m.matrix, m.rows, m.cols, rank);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = static_cast<int64_t>(rank);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_EIGEN: {
        Value m = read_matrix_strict(*this, y);
        if (m.rows != 2 || m.cols != 2) {
            throw_exec_error(*this, "MAT_EIGEN currently supports only 2x2 matrices", &ins);
        }
        const double a = m.matrix[0];
        const double b = m.matrix[1];
        const double c = m.matrix[2];
        const double d = m.matrix[3];
        const double tr = a + d;
        const double det = a * d - b * c;
        const double disc = tr * tr - 4.0 * det;
        if (disc < 0.0) {
            throw_exec_error(*this, "MAT_EIGEN does not support complex eigenvalues", &ins);
        }
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec = {(tr + std::sqrt(disc)) / 2.0, (tr - std::sqrt(disc)) / 2.0};
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::MAT_SOLVE: {
        Value a = read_matrix_strict(*this, y);
        Value b = read_vector_strict(*this, z);
        if (a.rows != a.cols) {
            throw_exec_error(*this, "MAT_SOLVE requires a square coefficient matrix", &ins);
        }
        if (b.vec.size() != a.rows) {
            throw_exec_error(*this, "MAT_SOLVE RHS vector size must equal matrix row count", &ins);
        }
        const size_t n = a.rows;
        std::vector<double> aug(n * (n + 1), 0.0);
        for (size_t r = 0; r < n; r++) {
            for (size_t c = 0; c < n; c++) {
                aug[r * (n + 1) + c] = a.matrix[r * n + c];
            }
            aug[r * (n + 1) + n] = b.vec[r];
        }
        for (size_t pivot = 0; pivot < n; pivot++) {
            size_t best = pivot;
            double best_abs = std::fabs(aug[pivot * (n + 1) + pivot]);
            for (size_t r = pivot + 1; r < n; r++) {
                const double cand = std::fabs(aug[r * (n + 1) + pivot]);
                if (cand > best_abs) {
                    best_abs = cand;
                    best = r;
                }
            }
            if (best_abs == 0.0) {
                throw_exec_error(*this, "MAT_SOLVE matrix is singular", &ins);
            }
            if (best != pivot) {
                for (size_t c = 0; c <= n; c++) {
                    std::swap(aug[pivot * (n + 1) + c], aug[best * (n + 1) + c]);
                }
            }
            const double pivot_value = aug[pivot * (n + 1) + pivot];
            for (size_t c = 0; c <= n; c++) {
                aug[pivot * (n + 1) + c] /= pivot_value;
            }
            for (size_t r = 0; r < n; r++) {
                if (r == pivot) continue;
                const double factor = aug[r * (n + 1) + pivot];
                for (size_t c = 0; c <= n; c++) {
                    aug[r * (n + 1) + c] -= factor * aug[pivot * (n + 1) + c];
                }
            }
        }
        Value out{};
        out.kind = ValueKind::Vector;
        out.vec.resize(n);
        for (size_t i = 0; i < n; i++) {
            out.vec[i] = aug[i * (n + 1) + n];
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
        out.i = static_cast<int64_t>(in.s.length());

        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_SUBSTR: {
        Value in = read_string_strict(*this, y);
        size_t start = read_non_negative_integer_operand(*this, z, ins, "STR_SUBSTR start");
        if (start > in.s.size()) {
            throw_exec_error(*this, "STR_SUBSTR start out of bounds", &ins);
        }
        Value out{};
        out.kind = ValueKind::String;
        out.s = in.s.substr(start);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_FIND: {
        Value haystack = read_string_strict(*this, y);
        Value needle = read_string_strict(*this, z);
        Value out{};
        out.kind = ValueKind::Integer;
        size_t pos = haystack.s.find(needle.s);
        out.i = pos == std::string::npos ? -1 : static_cast<int64_t>(pos);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_REPLACE: {
        Value inout = read_string_strict(*this, x);
        Value from = read_string_strict(*this, y);
        Value to = read_string_strict(*this, z);
        if (!from.s.empty()) {
            size_t pos = 0;
            while ((pos = inout.s.find(from.s, pos)) != std::string::npos) {
                inout.s.replace(pos, from.s.size(), to.s);
                pos += to.s.size();
            }
        }
        write_operand(x, inout);
        pc++;
        return;
    }
    case OperationKind::STR_SPLIT: {
        Value in = read_string_strict(*this, y);
        Value delim = read_string_strict(*this, z);
        Value out{};
        out.kind = ValueKind::List;
        if (delim.s.empty()) {
            for (char ch : in.s) {
                Value item{};
                item.kind = ValueKind::String;
                item.s = std::string(1, ch);
                out.list.push_back(item);
            }
        } else {
            size_t start = 0;
            while (start <= in.s.size()) {
                size_t pos = in.s.find(delim.s, start);
                const size_t len = (pos == std::string::npos) ? (in.s.size() - start) : (pos - start);
                Value item{};
                item.kind = ValueKind::String;
                item.s = in.s.substr(start, len);
                out.list.push_back(item);
                if (pos == std::string::npos) {
                    break;
                }
                start = pos + delim.s.size();
            }
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_JOIN: {
        Value in = read_operand_strict(y, ValueKind::List);
        Value sep = read_string_strict(*this, z);
        Value out{};
        out.kind = ValueKind::String;
        for (size_t i = 0; i < in.list.size(); i++) {
            if (in.list[i].kind != ValueKind::String) {
                throw_exec_error(*this, "STR_JOIN expects a list of strings", &ins);
            }
            if (i > 0) {
                out.s += sep.s;
            }
            out.s += in.list[i].s;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_UPPER: {
        Value in = read_string_strict(*this, y);
        Value out = in;
        for (char& ch : out.s) {
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_LOWER: {
        Value in = read_string_strict(*this, y);
        Value out = in;
        for (char& ch : out.s) {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_TRIM: {
        Value in = read_string_strict(*this, y);
        Value out{};
        out.kind = ValueKind::String;
        out.s = trim_copy(in.s);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::STR_EQ: {
        Value lhs = read_string_strict(*this, y);
        Value rhs = read_string_strict(*this, z);
        write_operand(x, make_bool_value(lhs.s == rhs.s));
        pc++;
        return;
    }
    case OperationKind::TYPEOF: {
        Value in = read_operand(y);
        Value out{};
        out.kind = ValueKind::String;
        out.s = value_kind_to_string(in.kind);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::CAST_INT: {
        Value in = read_operand(y);
        Value out{};
        out.kind = ValueKind::Integer;
        if (in.kind == ValueKind::Integer) out.i = in.i;
        else if (in.kind == ValueKind::Float) out.i = static_cast<int64_t>(in.f);
        else if (in.kind == ValueKind::Boolean) out.i = in.b ? 1 : 0;
        else if (in.kind == ValueKind::String) out.i = std::stoll(in.s);
        else throw_exec_error(*this, "CAST_INT unsupported source type", &ins);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::CAST_FLOAT: {
        Value in = read_operand(y);
        Value out{};
        out.kind = ValueKind::Float;
        if (in.kind == ValueKind::Integer) out.f = static_cast<double>(in.i);
        else if (in.kind == ValueKind::Float) out.f = in.f;
        else if (in.kind == ValueKind::Boolean) out.f = in.b ? 1.0 : 0.0;
        else if (in.kind == ValueKind::String) out.f = std::stod(in.s);
        else throw_exec_error(*this, "CAST_FLOAT unsupported source type", &ins);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::CAST_BOOL: {
        write_operand(x, make_bool_value(value_truthy(read_operand(y))));
        pc++;
        return;
    }
    case OperationKind::CAST_STRING: {
        Value in = read_operand(y);
        Value out{};
        out.kind = ValueKind::String;
        out.s = in.to_str();
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::IS_NULL: {
        write_operand(x, make_bool_value(read_operand(y).kind == ValueKind::Null));
        pc++;
        return;
    }
    case OperationKind::IS_NAN: {
        Value in = read_operand(y);
        const bool is_nan = in.kind == ValueKind::Float && std::isnan(in.f);
        write_operand(x, make_bool_value(is_nan));
        pc++;
        return;
    }
    case OperationKind::IS_INF: {
        Value in = read_operand(y);
        const bool is_inf = in.kind == ValueKind::Float && std::isinf(in.f);
        write_operand(x, make_bool_value(is_inf));
        pc++;
        return;
    }
    case OperationKind::READ_FILE: {
        Value path = read_string_strict(*this, y);
        std::ifstream in(path.s, std::ios::binary);
        if (!in) {
            throw_exec_error(*this, "READ_FILE failed to open '" + path.s + "'", &ins);
        }
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        Value out{};
        out.kind = ValueKind::String;
        out.s = std::move(contents);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::WRITE_FILE: {
        Value path = read_string_strict(*this, x);
        Value content = read_string_strict(*this, y);
        std::ofstream out(path.s, std::ios::binary | std::ios::trunc);
        if (!out) {
            throw_exec_error(*this, "WRITE_FILE failed to open '" + path.s + "'", &ins);
        }
        out << content.s;
        pc++;
        return;
    }
    case OperationKind::APPEND_FILE: {
        Value path = read_string_strict(*this, x);
        Value content = read_string_strict(*this, y);
        std::ofstream out(path.s, std::ios::binary | std::ios::app);
        if (!out) {
            throw_exec_error(*this, "APPEND_FILE failed to open '" + path.s + "'", &ins);
        }
        out << content.s;
        pc++;
        return;
    }
    case OperationKind::FILE_EXISTS: {
        Value path = read_string_strict(*this, y);
        write_operand(x, make_bool_value(std::filesystem::exists(path.s)));
        pc++;
        return;
    }
    case OperationKind::DELETE_FILE: {
        Value path = read_string_strict(*this, x);
        (void)std::filesystem::remove(path.s);
        pc++;
        return;
    }
    case OperationKind::TIME_NOW: {
        const auto now = std::chrono::system_clock::now().time_since_epoch();
        Value out{};
        out.kind = ValueKind::Float;
        out.f = std::chrono::duration<double>(now).count();
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::SLEEP: {
        const double seconds = numeric_value_as_double(*this, x, ins, "SLEEP seconds");
        if (seconds < 0.0) {
            throw_exec_error(*this, "SLEEP duration must be non-negative", &ins);
        }
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
        pc++;
        return;
    }
    case OperationKind::JMP_IF: {
        if (value_truthy(read_operand(y))) {
            jump_to_label(*this, x, ins, "JMP_IF");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::JMP_IF_ZERO: {
        const double value = numeric_value_as_double(*this, y, ins, "JMP_IF_ZERO value");
        if (value == 0.0) {
            jump_to_label(*this, x, ins, "JMP_IF_ZERO");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::JMP_IF_NOT: {
        if (!value_truthy(read_operand(y))) {
            jump_to_label(*this, x, ins, "JMP_IF_NOT");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::JMP_EQ: {
        if (value_equals(read_operand(y), read_operand(z))) {
            jump_to_label(*this, x, ins, "JMP_EQ");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::JMP_NEQ: {
        if (!value_equals(read_operand(y), read_operand(z))) {
            jump_to_label(*this, x, ins, "JMP_NEQ");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::JMP_LT: {
        if (numeric_value_as_double(*this, y, ins, "JMP_LT") < numeric_value_as_double(*this, z, ins, "JMP_LT")) {
            jump_to_label(*this, x, ins, "JMP_LT");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::JMP_LTE: {
        if (numeric_value_as_double(*this, y, ins, "JMP_LTE") <= numeric_value_as_double(*this, z, ins, "JMP_LTE")) {
            jump_to_label(*this, x, ins, "JMP_LTE");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::JMP_GT: {
        if (numeric_value_as_double(*this, y, ins, "JMP_GT") > numeric_value_as_double(*this, z, ins, "JMP_GT")) {
            jump_to_label(*this, x, ins, "JMP_GT");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::JMP_GTE: {
        if (numeric_value_as_double(*this, y, ins, "JMP_GTE") >= numeric_value_as_double(*this, z, ins, "JMP_GTE")) {
            jump_to_label(*this, x, ins, "JMP_GTE");
        } else {
            pc++;
        }
        return;
    }
    case OperationKind::STORE: {
        if (x.kind != OperandKind::Slot) {
            throw_exec_error(*this, "STORE expects Slot destination", &ins);
        }
        slot(x.value) = read_operand(y);
        pc++;
        return;
    }
    case OperationKind::COPY: {
        write_operand(x, read_operand(y));
        pc++;
        return;
    }
    case OperationKind::SWAP: {
        Value lhs = read_operand(x);
        Value rhs = read_operand(y);
        write_operand(x, rhs);
        write_operand(y, lhs);
        pc++;
        return;
    }
    case OperationKind::CLEAR: {
        Value out{};
        out.kind = ValueKind::Null;
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::SIGN: {
        const double v = numeric_value_as_double(*this, y, ins, "SIGN");
        write_operand(x, make_numeric_value(v > 0.0 ? 1.0 : (v < 0.0 ? -1.0 : 0.0)));
        pc++;
        return;
    }
    case OperationKind::CBRT: {
        write_operand(x, make_numeric_value(std::cbrt(numeric_value_as_double(*this, y, ins, "CBRT"))));
        pc++;
        return;
    }
    case OperationKind::FACTORIAL: {
        Value n = read_operand_strict(y, ValueKind::Integer);
        if (n.i < 0) {
            throw_exec_error(*this, "FACTORIAL requires non-negative integer", &ins);
        }
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = 1;
        for (int64_t i = 2; i <= n.i; i++) {
            out.i *= i;
        }
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::GCD: {
        Value a = read_operand_strict(y, ValueKind::Integer);
        Value b = read_operand_strict(z, ValueKind::Integer);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = std::gcd(a.i, b.i);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::LCM: {
        Value a = read_operand_strict(y, ValueKind::Integer);
        Value b = read_operand_strict(z, ValueKind::Integer);
        Value out{};
        out.kind = ValueKind::Integer;
        out.i = std::lcm(a.i, b.i);
        write_operand(x, out);
        pc++;
        return;
    }
    case OperationKind::REM: {
        write_operand(x, make_numeric_value(std::remainder(numeric_value_as_double(*this, y, ins, "REM"),
                                                           numeric_value_as_double(*this, z, ins, "REM"))));
        pc++;
        return;
    }
    case OperationKind::FMOD: {
        write_operand(x, make_numeric_value(std::fmod(numeric_value_as_double(*this, y, ins, "FMOD"),
                                                      numeric_value_as_double(*this, z, ins, "FMOD"))));
        pc++;
        return;
    }
    case OperationKind::DEG2RAD: {
        write_operand(x, make_numeric_value(numeric_value_as_double(*this, y, ins, "DEG2RAD") * std::numbers::pi / 180.0));
        pc++;
        return;
    }
    case OperationKind::RAD2DEG: {
        write_operand(x, make_numeric_value(numeric_value_as_double(*this, y, ins, "RAD2DEG") * 180.0 / std::numbers::pi));
        pc++;
        return;
    }
    case OperationKind::LERP: {
        const double t = std::clamp(numeric_value_as_double(*this, x, ins, "LERP t"), 0.0, 1.0);
        const double a = numeric_value_as_double(*this, y, ins, "LERP a");
        const double b = numeric_value_as_double(*this, z, ins, "LERP b");
        write_operand(x, make_numeric_value(a + (b - a) * t));
        pc++;
        return;
    }
    case OperationKind::MAP_RANGE: {
        const double value = numeric_value_as_double(*this, x, ins, "MAP_RANGE value");
        const double in_min = numeric_value_as_double(*this, y, ins, "MAP_RANGE in_min");
        const double in_max = numeric_value_as_double(*this, z, ins, "MAP_RANGE in_max");
        if (in_max == in_min) {
            throw_exec_error(*this, "MAP_RANGE requires in_min != in_max", &ins);
        }
        write_operand(x, make_numeric_value((value - in_min) / (in_max - in_min)));
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

        if (minv.i > maxv.i) {
            throw_exec_error(*this, "RAND_INT requires min <= max", &ins);
        }

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

void Program::exec(const ExecutionOptions& options) {
    const auto main_index = functions.try_index_of("main");
    if (!main_index.has_value()) {
        throw std::runtime_error(format_error_context(ErrorPhase::Exec,
                                                      "Missing '.main' entry point"));
    }

    pc = 0;
    halted = false;
    fc = *main_index;
    call_stack.clear();
    stack.clear();

    Function* main_function = functions.at(fc);
    if (main_function->ins.empty()) {
        throw std::runtime_error("Runtime error: '.main' function contains no instructions");
    }

    const bool collect_timing = options.profile || options.trace_time;
    const bool collect_opcode_counts = options.opcode_stats || options.profile;

    size_t loaded_fc = static_cast<size_t>(-1);
    Function* current_function = nullptr;

    while (!halted) {
        if (fc >= functions.size()) {
            throw runtime_error_with_context(nullptr, pc, std::format("Invalid function index {}", fc));
        }

        if (loaded_fc != fc) {
            current_function = functions.at(fc);
            loaded_fc = fc;
        }
        if (pc >= current_function->ins.size()) {
            throw runtime_error_with_context(
                current_function,
                pc,
                std::format("Instruction pointer out of range (function size={})", current_function->ins.size()));
        }

        const size_t before_pc = pc;
        const Instruction& ins = current_function->ins[before_pc];

        const auto start = collect_timing ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};
        resolve(ins);
        const auto end = collect_timing ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point{};

        if (collect_opcode_counts && options.stats != nullptr) {
            options.stats->opcode_counts[ins.op]++;
        }

        if (collect_timing) {
            const auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            if (options.profile && options.stats != nullptr) {
                options.stats->opcode_time[ins.op] += dt;
                options.stats->function_time[current_function->name] += dt;
                options.stats->function_instruction_counts[current_function->name]++;
            }

            if (options.trace_time) {
                std::cout << "trace fn=" << current_function->name
                          << " pc=" << before_pc
                          << " op=" << operation_kind_to_string(ins.op)
                          << " time_ns=" << dt.count()
                          << std::endl;
            }
        }
    }
}

void Program::exec() {
    exec(ExecutionOptions{});
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
