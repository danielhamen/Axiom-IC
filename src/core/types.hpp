#pragma once

#include <cstdint>
#include <format>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace aic {

enum class Directive : int8_t {
    Constant,
    Function,
    None,
    Include
};

enum class OperationKind : int16_t {
    /**
     * ARITHMETIC
     */
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,

    /**
     * CONTROL FLOW
     */
    HALT,
    JMP,

    /** I/O */
    PRINT,

    /** FUNCTIONS */
    PUSH,
    POP,
    CALL,
    RET,
    RETVAL,

    /** LIST OPERATIONS */
    LIST_NEW,
    LIST_PUSH,
    LIST_GET,
    LIST_SET,
    LIST_LEN,
    INT_LIST_NEW,
    INT_LIST_PUSH,
    INT_LIST_GET,
    INT_LIST_SET,
    INT_LIST_LEN,
    FLOAT_LIST_NEW,
    FLOAT_LIST_PUSH,
    FLOAT_LIST_GET,
    FLOAT_LIST_SET,
    FLOAT_LIST_LEN,
    VEC_NEW,
    VEC_PUSH,
    VEC_GET,
    VEC_SET,
    VEC_LEN,
    VEC_ADD,
    VEC_SUB,
    VEC_SCALE,
    VEC_DOT,
    VEC_CROSS,
    VEC_MAG,
    VEC_NORM,
    MAT_NEW,
    MAT_GET,
    MAT_SET,
    MAT_ROWS,
    MAT_COLS,
    MAT_MUL,
    MAT_TRANSPOSE,
    MAT_DET,
    MAT_INV
};

enum class ErrorPhase : uint8_t {
    Tokenize,
    Parse,
    Exec
};

struct Operation {
    OperationKind kind;
    std::string name;
    size_t arity;
};

char escape_char(char in);

enum class OperandKind : uint8_t {
    None,
    Slot,
    Immediate,
    Constant,
    Label,
    Address,
    Function,
    Argument
};

extern std::unordered_map<std::string, OperandKind> operand_kind_map;

enum class ValueKind : uint8_t {
    Integer,
    Float,
    String,
    Boolean,
    Null,
    List,
    IntegerList,
    FloatList,
    Vector,
    Matrix
};

struct Value {
    ValueKind kind;
    std::string s{};
    int64_t i = 0;
    double f = 0.0;
    bool b = false;
    std::vector<Value> list{};
    std::vector<int64_t> int_list{};
    std::vector<double> float_list{};
    std::vector<double> vec{};
    std::vector<double> matrix{};
    size_t rows = 0;
    size_t cols = 0;

    bool is_str() const { return kind == ValueKind::String; }
    bool is_int() const { return kind == ValueKind::Integer; }
    bool is_float() const { return kind == ValueKind::Float; }
    bool is_bool() const { return kind == ValueKind::Boolean; }
    bool is_null() const { return kind == ValueKind::Null; }
    bool is_list() const { return kind == ValueKind::List; }
    bool is_int_list() const { return kind == ValueKind::IntegerList; }
    bool is_float_list() const { return kind == ValueKind::FloatList; }
    bool is_vec() const { return kind == ValueKind::Vector; }
    bool is_matrix() const { return kind == ValueKind::Matrix; }

    std::string to_str() const {
        switch (kind) {
            case ValueKind::String:
                return s;
            case ValueKind::Boolean:
                return b ? "true" : "false";
            case ValueKind::Null:
                return "null";
            case ValueKind::Float:
                return std::to_string(f);
            case ValueKind::Integer:
                return std::to_string(i);
            case ValueKind::List: {
                std::string out = "[";
                for (size_t idx = 0; idx < list.size(); idx++) {
                    if (idx > 0) {
                        out += ", ";
                    }
                    out += list[idx].to_str();
                }
                out += "]";
                return out;
            }
            case ValueKind::IntegerList: {
                std::string out = "[";
                for (size_t idx = 0; idx < int_list.size(); idx++) {
                    if (idx > 0) out += ", ";
                    out += std::to_string(int_list[idx]);
                }
                out += "]";
                return out;
            }
            case ValueKind::FloatList: {
                std::string out = "[";
                for (size_t idx = 0; idx < float_list.size(); idx++) {
                    if (idx > 0) out += ", ";
                    out += std::to_string(float_list[idx]);
                }
                out += "]";
                return out;
            }
            case ValueKind::Vector: {
                std::string out = "vec(";
                for (size_t idx = 0; idx < vec.size(); idx++) {
                    if (idx > 0) out += ", ";
                    out += std::to_string(vec[idx]);
                }
                out += ")";
                return out;
            }
            case ValueKind::Matrix: {
                std::string out = "mat(";
                out += std::to_string(rows);
                out += "x";
                out += std::to_string(cols);
                out += ")[";
                for (size_t r = 0; r < rows; r++) {
                    if (r > 0) out += "; ";
                    out += "[";
                    for (size_t c = 0; c < cols; c++) {
                        if (c > 0) out += ", ";
                        out += std::to_string(matrix[r * cols + c]);
                    }
                    out += "]";
                }
                out += "]";
                return out;
            }
        }

        throw std::runtime_error("Unmatched ValueKind");
    }
};

struct Operand {
    OperandKind kind;
    int64_t value{};
    std::string strval;
    Value immediate{};
    bool has_immediate = false;

    bool is_none() const {
        return kind == OperandKind::None;
    }

    std::string to_str() const;

    std::string kindstr() const {
        for (auto entry : operand_kind_map) {
            if (entry.second == kind)
                return entry.first;
        }

        throw std::runtime_error("Unmatched OperandKind");
    }
};

struct Instruction {
    OperationKind op;
    Operand x;
    Operand y;
    Operand z;

    std::string to_string() const;
};

std::string operation_kind_to_string(OperationKind kind);
std::string value_kind_to_string(ValueKind kind);
std::string error_phase_to_string(ErrorPhase phase);
std::string format_error_context(ErrorPhase phase,
                                 const std::string& message,
                                 int line = -1,
                                 int column = -1,
                                 const std::string& function_name = "",
                                 const std::string& opcode = "",
                                 size_t pc = static_cast<size_t>(-1));

struct Function {
    std::vector<Instruction> ins;
    std::string name;
    std::unordered_map<std::string, size_t> labels;
    size_t arg_count = 0;
};

class FunctionList {
private:
    std::vector<Function> functions;

public:
    bool exists(std::string fn_name);
    Function* find(std::string fn_name);
    Function* at(size_t idx);
    size_t index_of(std::string fn_name);
    void insert(Function& fn);
    const size_t size();
};

} // namespace aic
