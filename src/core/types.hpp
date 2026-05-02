#pragma once

#include <cstdint>
#include <map>
#include <optional>
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
     * ==========
     * ARITHMETIC
     * ==========
     */
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NEG,
    INC,
    DEC,
    ABS,
    EQ,
    NEQ,
    LT,
    LTE,
    GT,
    GTE,
    AND,
    OR,
    NOT,
    XOR,
    POW,
    SQRT,
    EXP,
    LOG,
    LOG2,
    LOG10,
    LN,
    SIN,
    COS,
    TAN,
    CSC,
    SEC,
    COT,
    ASIN,
    ACOS,
    ATAN,
    ACSC,
    ASEC,
    ACOT,
    SINH,
    COSH,
    TANH,
    CSCH,
    SECH,
    COTH,
    ASINH,
    ACOSH,
    ATANH,
    ACSCH,
    ASECH,
    ACOTH,
    ATAN2,
    HYPOT,
    FLOOR,
    CEIL,
    ROUND,
    MIN,
    MAX,
    CLAMP,

    /**
     * ============
     * CONTROL FLOW
     * ============
     */
    HALT,
    JMP,

    /**
     * =====
     *  I/O
     * =====
     */
    PRINT,
    PRINTLN,


    /**
     * =========
     * FUNCTIONS
     * =========
     */
    PUSH,
    POP,
    CALL,
    RET,
    RETVAL,
    ARG,
    KWARG,
    ARG_ARITY,
    KWARG_ARITY,
    ARG_GET,
    KWARG_GET,
    KWARG_HAS,
    ARG_REQUIRE,
    KWARG_REQUIRE,


    /**
     * ===============
     * LIST OPERATIONS
     * ===============
     */
    LIST_NEW,
    LIST_PUSH,
    LIST_POP,
    LIST_INSERT,
    LIST_ERASE,
    LIST_SLICE,
    LIST_CLEAR,
    LIST_FIND,
    LIST_SORT,
    LIST_REVERSE,
    LIST_GET,
    LIST_SET,
    LIST_LEN,

    /**
     * ==============
     * MAP OPERATIONS
     * ==============
     */
    MAP_NEW,
    MAP_SET,
    MAP_GET,
    MAP_HAS,
    MAP_DELETE,
    MAP_KEYS,
    MAP_VALUES,

    /**
     * ==============
     * SET OPERATIONS
     * ==============
     */
    SET_NEW,
    SET_ADD,
    SET_HAS,
    SET_DELETE,
    SET_UNION,
    SET_INTERSECT,

    /**
     * =======
     * VECTORS
     * =======
     */
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
    VEC_PROJECT,
    VEC_REFLECT,
    VEC_ANGLE,
    VEC_DISTANCE,
    VEC_LERP,

    /**
     * ========
     * MATRICES
     * ========
     */
    MAT_NEW,
    MAT_GET,
    MAT_SET,
    MAT_ROWS,
    MAT_COLS,
    MAT_MUL,
    MAT_ADD,
    MAT_SUB,
    MAT_SCALE,
    MAT_IDENTITY,
    MAT_TRACE,
    MAT_RREF,
    MAT_RANK,
    MAT_EIGEN,
    MAT_SOLVE,
    MAT_TRANSPOSE,
    MAT_DET,
    MAT_INV,

    /**
     * =======
     * STRINGS
     * =======
     */
    STR_LEN,
    STR_CONCAT,
    STR_SUBSTR,
    STR_FIND,
    STR_REPLACE,
    STR_SPLIT,
    STR_JOIN,
    STR_UPPER,
    STR_LOWER,
    STR_TRIM,
    STR_EQ,

    /**
     * =========
     * TYPE/CAST
     * =========
     */
    TYPEOF,
    CAST_INT,
    CAST_FLOAT,
    CAST_BOOL,
    CAST_STRING,
    IS_NULL,
    IS_NAN,
    IS_INF,

    /**
     * =========
     * FILE/TIME
     * =========
     */
    READ_FILE,
    WRITE_FILE,
    APPEND_FILE,
    FILE_EXISTS,
    DELETE_FILE,
    TIME_NOW,
    SLEEP,

    /**
     * ============
     * CONTROL FLOW
     * ============
     */
    JMP_IF,
    JMP_IF_ZERO,
    JMP_IF_NOT,
    JMP_EQ,
    JMP_NEQ,
    JMP_LT,
    JMP_LTE,
    JMP_GT,
    JMP_GTE,

    /**
     * ============
     * MEMORY/STACK
     * ============
     */
    COPY,
    STORE,
    SWAP,
    CLEAR,

    /**
     * ==========
     * EXTRA MATH
     * ==========
     */
    SIGN,
    CBRT,
    FACTORIAL,
    GCD,
    LCM,
    REM,
    FMOD,
    DEG2RAD,
    RAD2DEG,
    LERP,
    MAP_RANGE,

    /**
     * ======
     * RANDOM
     * ======
     */
    RAND_RANDOM,
    RAND_INT,
    RAND_RANGE,
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
    Map,
    Set,
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
    std::map<std::string, Value> map{};
    std::vector<Value> set{};
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
    bool is_map() const { return kind == ValueKind::Map; }
    bool is_set() const { return kind == ValueKind::Set; }
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
            case ValueKind::Map: {
                std::string out = "{";
                size_t idx = 0;
                for (const auto& [key, value] : map) {
                    if (idx > 0) {
                        out += ", ";
                    }
                    out += key;
                    out += ": ";
                    out += value.to_str();
                    idx++;
                }
                out += "}";
                return out;
            }
            case ValueKind::Set: {
                std::string out = "set{";
                for (size_t idx = 0; idx < set.size(); idx++) {
                    if (idx > 0) {
                        out += ", ";
                    }
                    out += set[idx].to_str();
                }
                out += "}";
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
    std::unordered_map<std::string, size_t> function_indices;

public:
    bool exists(const std::string& fn_name) const;
    Function* find(const std::string& fn_name);
    const Function* find(const std::string& fn_name) const;
    Function* at(size_t idx);
    const Function* at(size_t idx) const;
    std::optional<size_t> try_index_of(const std::string& fn_name) const;
    size_t index_of(const std::string& fn_name) const;
    void insert(const Function& fn);
    void insert(Function&& fn);
    size_t size() const;
};

} // namespace aic
