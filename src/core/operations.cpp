#include "operations.hpp"

#include <array>
#include <stdexcept>
#include <unordered_set>

namespace aic {

namespace {

constexpr std::array<OperationDefinition, 113> kOperationTable{{
    // ARITHMETIC
    {OperationKind::ADD, "ADD", 3, "arithmetic"},
    {OperationKind::SUB, "SUB", 3, "arithmetic"},
    {OperationKind::MUL, "MUL", 3, "arithmetic"},
    {OperationKind::DIV, "DIV", 3, "arithmetic"},
    {OperationKind::MOD, "MOD", 3, "arithmetic"},
    {OperationKind::NEG, "NEG", 2, "arithmetic"},
    {OperationKind::INC, "INC", 2, "arithmetic"},
    {OperationKind::DEC, "DEC", 2, "arithmetic"},
    {OperationKind::ABS, "ABS", 2, "arithmetic"},
    {OperationKind::EQ, "EQ", 3, "comparison"},
    {OperationKind::NEQ, "NEQ", 3, "comparison"},
    {OperationKind::LT, "LT", 3, "comparison"},
    {OperationKind::LTE, "LTE", 3, "comparison"},
    {OperationKind::GT, "GT", 3, "comparison"},
    {OperationKind::GTE, "GTE", 3, "comparison"},
    {OperationKind::AND, "AND", 3, "logic"},
    {OperationKind::OR, "OR", 3, "logic"},
    {OperationKind::NOT, "NOT", 2, "logic"},
    {OperationKind::XOR, "XOR", 3, "logic"},
    {OperationKind::POW, "POW", 3, "math"},
    {OperationKind::SQRT, "SQRT", 2, "math"},
    {OperationKind::EXP, "EXP", 2, "math"},
    {OperationKind::LOG, "LOG", 3, "math"},
    {OperationKind::LOG2, "LOG2", 2, "math"},
    {OperationKind::LOG10, "LOG10", 2, "math"},
    {OperationKind::LN, "LN", 2, "math"},
    {OperationKind::SIN, "SIN", 2, "math"},
    {OperationKind::COS, "COS", 2, "math"},
    {OperationKind::TAN, "TAN", 2, "math"},
    {OperationKind::CSC, "CSC", 2, "math"},
    {OperationKind::SEC, "SEC", 2, "math"},
    {OperationKind::COT, "COT", 2, "math"},
    {OperationKind::ASIN, "ASIN", 2, "math"},
    {OperationKind::ACOS, "ACOS", 2, "math"},
    {OperationKind::ATAN, "ATAN", 2, "math"},
    {OperationKind::ACSC, "ACSC", 2, "math"},
    {OperationKind::ASEC, "ASEC", 2, "math"},
    {OperationKind::ACOT, "ACOT", 2, "math"},
    {OperationKind::SINH, "SINH", 2, "math"},
    {OperationKind::COSH, "COSH", 2, "math"},
    {OperationKind::TANH, "TANH", 2, "math"},
    {OperationKind::CSCH, "CSCH", 2, "math"},
    {OperationKind::SECH, "SECH", 2, "math"},
    {OperationKind::COTH, "COTH", 2, "math"},
    {OperationKind::ASINH, "ASINH", 2, "math"},
    {OperationKind::ACOSH, "ACOSH", 2, "math"},
    {OperationKind::ATANH, "ATANH", 2, "math"},
    {OperationKind::ACSCH, "ACSCH", 2, "math"},
    {OperationKind::ASECH, "ASECH", 2, "math"},
    {OperationKind::ACOTH, "ACOTH", 2, "math"},
    {OperationKind::ATAN2, "ATAN2", 3, "math"},
    {OperationKind::HYPOT, "HYPOT", 3, "math"},
    {OperationKind::FLOOR, "FLOOR", 2, "math"},
    {OperationKind::CEIL, "CEIL", 2, "math"},
    {OperationKind::ROUND, "ROUND", 2, "math"},
    {OperationKind::MIN, "MIN", 3, "math"},
    {OperationKind::MAX, "MAX", 3, "math"},
    {OperationKind::CLAMP, "CLAMP", 3, "math"},

    // CONTROL FLOW
    {OperationKind::JMP, "JMP", 1, "control_flow"},
    {OperationKind::HALT, "HALT", 0, "control_flow"},

    // IO
    {OperationKind::PRINT, "PRINT", 1, "io"},

    // FUNCTIONS & STACK
    {OperationKind::PUSH, "PUSH", 1, "stack"},
    {OperationKind::POP, "POP", 1, "stack"},
    {OperationKind::CALL, "CALL", 1, "function"},
    {OperationKind::RET, "RET", 0, "function"},
    {OperationKind::RETVAL, "RETVAL", 1, "function"},

    // LISTS
    {OperationKind::LIST_NEW, "LIST_NEW", 2, "list"},
    {OperationKind::LIST_PUSH, "LIST_PUSH", 2, "list"},
    {OperationKind::LIST_GET, "LIST_GET", 3, "list"},
    {OperationKind::LIST_SET, "LIST_SET", 3, "list"},
    {OperationKind::LIST_LEN, "LIST_LEN", 2, "list"},

    // INTEGER LIST
    {OperationKind::INT_LIST_NEW, "INT_LIST_NEW", 2, "list"},
    {OperationKind::INT_LIST_PUSH, "INT_LIST_PUSH", 2, "list"},
    {OperationKind::INT_LIST_GET, "INT_LIST_GET", 3, "list"},
    {OperationKind::INT_LIST_SET, "INT_LIST_SET", 3, "list"},
    {OperationKind::INT_LIST_LEN, "INT_LIST_LEN", 2, "list"},

    // FLOAT LIST
    {OperationKind::FLOAT_LIST_NEW, "FLOAT_LIST_NEW", 2, "list"},
    {OperationKind::FLOAT_LIST_PUSH, "FLOAT_LIST_PUSH", 2, "list"},
    {OperationKind::FLOAT_LIST_GET, "FLOAT_LIST_GET", 3, "list"},
    {OperationKind::FLOAT_LIST_SET, "FLOAT_LIST_SET", 3, "list"},
    {OperationKind::FLOAT_LIST_LEN, "FLOAT_LIST_LEN", 2, "list"},

    // VECTOR
    {OperationKind::VEC_NEW, "VEC_NEW", 2, "vector"},
    {OperationKind::VEC_PUSH, "VEC_PUSH", 2, "vector"},
    {OperationKind::VEC_GET, "VEC_GET", 3, "vector"},
    {OperationKind::VEC_SET, "VEC_SET", 3, "vector"},
    {OperationKind::VEC_LEN, "VEC_LEN", 2, "vector"},
    {OperationKind::VEC_ADD, "VEC_ADD", 3, "vector"},
    {OperationKind::VEC_SUB, "VEC_SUB", 3, "vector"},
    {OperationKind::VEC_SCALE, "VEC_SCALE", 3, "vector"},
    {OperationKind::VEC_DOT, "VEC_DOT", 3, "vector"},
    {OperationKind::VEC_CROSS, "VEC_CROSS", 3, "vector"},
    {OperationKind::VEC_MAG, "VEC_MAG", 2, "vector"},
    {OperationKind::VEC_NORM, "VEC_NORM", 2, "vector"},

    // MATRIX
    {OperationKind::MAT_NEW, "MAT_NEW", 3, "matrix"},
    {OperationKind::MAT_GET, "MAT_GET", 3, "matrix"},
    {OperationKind::MAT_SET, "MAT_SET", 3, "matrix"},
    {OperationKind::MAT_ROWS, "MAT_ROWS", 2, "matrix"},
    {OperationKind::MAT_COLS, "MAT_COLS", 2, "matrix"},
    {OperationKind::MAT_MUL, "MAT_MUL", 3, "matrix"},
    {OperationKind::MAT_TRANSPOSE, "MAT_TRANSPOSE", 2, "matrix"},
    {OperationKind::MAT_DET, "MAT_DET", 2, "matrix"},
    {OperationKind::MAT_INV, "MAT_INV", 2, "matrix"},

    // STRINGS
    {OperationKind::STR_CONCAT, "STR_CONCAT", 3, "string"},
    {OperationKind::STR_LEN, "STR_LEN", 2, "string"},

    // RANDOM
    {OperationKind::RAND_RANDOM, "RAND_RANDOM", 1, "random"},
    {OperationKind::RAND_INT, "RAND_INT", 3, "random"},
    {OperationKind::RAND_RANGE, "RAND_RANGE", 3, "random"},
}};

} // namespace

std::unordered_map<std::string, Operation> operation_by_name;

std::span<const OperationDefinition> operation_definitions() {
    return kOperationTable;
}

void register_operations() {
    if (!operation_by_name.empty()) {
        return;
    }

    operation_by_name.reserve(kOperationTable.size());

    std::unordered_set<std::string> seen_names;
    std::unordered_set<int> seen_kinds;
    seen_names.reserve(kOperationTable.size());
    seen_kinds.reserve(kOperationTable.size());

    for (const auto& definition : kOperationTable) {
        const auto [_, inserted_name] = seen_names.emplace(definition.name);
        if (!inserted_name) {
            throw std::runtime_error("Duplicate operation name: " + std::string(definition.name));
        }

        const auto [__, inserted_kind] = seen_kinds.emplace(static_cast<int>(definition.kind));
        if (!inserted_kind) {
            throw std::runtime_error(
                "Duplicate operation kind for opcode: " + std::string(definition.name)
            );
        }

        operation_by_name.emplace(
            definition.name,
            Operation{definition.kind, definition.name, definition.arity}
        );
    }
}

} // namespace aic
