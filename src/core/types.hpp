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
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    HALT,
    JMP,
    PRINT,
    PUSH,
    POP,
    CALL,
    RET,
    RETVAL,
    LIST_NEW,
    LIST_PUSH,
    LIST_GET,
    LIST_SET,
    LIST_LEN
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
    List
};

struct Value {
    ValueKind kind;
    std::string s{};
    int64_t i = 0;
    double f = 0.0;
    bool b = false;
    std::vector<Value> list{};

    bool is_str() const { return kind == ValueKind::String; }
    bool is_int() const { return kind == ValueKind::Integer; }
    bool is_float() const { return kind == ValueKind::Float; }
    bool is_bool() const { return kind == ValueKind::Boolean; }
    bool is_null() const { return kind == ValueKind::Null; }
    bool is_list() const { return kind == ValueKind::List; }

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
