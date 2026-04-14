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
    PRINT
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
    Address
};

extern std::unordered_map<std::string, OperandKind> operand_kind_map;

enum class ValueKind : uint8_t {
    Integer,
    Float,
    String,
    Boolean,
    Null
};

struct Value {
    ValueKind kind;
    std::string s{};
    int64_t i = 0;
    double f = 0.0;
    bool b = false;

    bool is_str() const { return kind == ValueKind::String; }
    bool is_int() const { return kind == ValueKind::Integer; }
    bool is_float() const { return kind == ValueKind::Float; }
    bool is_bool() const { return kind == ValueKind::Boolean; }
    bool is_null() const { return kind == ValueKind::Null; }

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

    std::string to_str() const {
        return std::format("Operand(kind={0}, value={1}, strval=2)", kindstr(), value, strval);
    }

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
};

struct Function {
    std::vector<Instruction> ins;
    std::string name;
    std::unordered_map<std::string, size_t> labels;
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
