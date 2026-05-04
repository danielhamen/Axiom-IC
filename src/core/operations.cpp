#include "operations.hpp"

#include <array>
#include <stdexcept>
#include <unordered_set>

namespace aic {

namespace {

constexpr auto kOperationTable = std::to_array<OperationDefinition>({
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

    // EXCEPTIONS
    {OperationKind::TRY, "TRY", 1, "exception"},
    {OperationKind::CATCH, "CATCH", 0, "exception"},
    {OperationKind::FINALLY, "FINALLY", 1, "exception"},
    {OperationKind::END_TRY, "END_TRY", 0, "exception"},
    {OperationKind::THROW, "THROW", 1, "exception"},
    {OperationKind::ERR_GET, "ERR_GET", 1, "exception"},
    {OperationKind::ERR_CLEAR, "ERR_CLEAR", 0, "exception"},
    {OperationKind::ERROR_NEW, "ERROR_NEW", 3, "exception"},
    {OperationKind::ERROR_TYPE, "ERROR_TYPE", 2, "exception"},
    {OperationKind::ERROR_MESSAGE, "ERROR_MESSAGE", 2, "exception"},
    {OperationKind::ERROR_IS, "ERROR_IS", 3, "exception"},

    // LITERALS
    {OperationKind::LITERAL, "LITERAL", 2, "literal"},

    // CONTROL FLOW
    {OperationKind::JMP, "JMP", 1, "control_flow"},
    {OperationKind::HALT, "HALT", 0, "control_flow"},

    // IO
    {OperationKind::PRINT, "PRINT", 1, "io"},
    {OperationKind::PRINTLN, "PRINTLN", 1, "io"},

    // FUNCTIONS & STACK
    {OperationKind::PUSH, "PUSH", 1, "stack"},
    {OperationKind::POP, "POP", 1, "stack"},
    {OperationKind::CALL, "CALL", 1, "function"},
    {OperationKind::RET, "RET", 0, "function"},
    {OperationKind::RETVAL, "RETVAL", 1, "function"},
    {OperationKind::ARG, "ARG", 1, "function"},
    {OperationKind::KWARG, "KWARG", 2, "function"},
    {OperationKind::ARG_ARITY, "ARG_ARITY", 1, "function"},
    {OperationKind::KWARG_ARITY, "KWARG_ARITY", 1, "function"},
    {OperationKind::ARG_GET, "ARG_GET", 2, "function"},
    {OperationKind::KWARG_GET, "KWARG_GET", 2, "function"},
    {OperationKind::KWARG_HAS, "KWARG_HAS", 2, "function"},
    {OperationKind::ARG_REQUIRE, "ARG_REQUIRE", 2, "function"},
    {OperationKind::KWARG_REQUIRE, "KWARG_REQUIRE", 2, "function"},

    // LISTS
    {OperationKind::LIST_NEW, "LIST_NEW", 2, "list"},
    {OperationKind::LIST_PUSH, "LIST_PUSH", 2, "list"},
    {OperationKind::LIST_POP, "LIST_POP", 2, "list"},
    {OperationKind::LIST_INSERT, "LIST_INSERT", 3, "list"},
    {OperationKind::LIST_ERASE, "LIST_ERASE", 2, "list"},
    {OperationKind::LIST_SLICE, "LIST_SLICE", 3, "list"},
    {OperationKind::LIST_CLEAR, "LIST_CLEAR", 1, "list"},
    {OperationKind::LIST_FIND, "LIST_FIND", 3, "list"},
    {OperationKind::LIST_SORT, "LIST_SORT", 1, "list"},
    {OperationKind::LIST_REVERSE, "LIST_REVERSE", 1, "list"},
    {OperationKind::LIST_GET, "LIST_GET", 3, "list"},
    {OperationKind::LIST_SET, "LIST_SET", 3, "list"},
    {OperationKind::LIST_LEN, "LIST_LEN", 2, "list"},
    {OperationKind::LIST_MAP, "LIST_MAP", 3, "list"},
    {OperationKind::LIST_FILTER, "LIST_FILTER", 3, "list"},
    {OperationKind::LIST_REDUCE, "LIST_REDUCE", 4, "list"},
    {OperationKind::LIST_CONCAT, "LIST_CONCAT", 3, "list"},
    {OperationKind::LIST_CLONE, "LIST_CLONE", 2, "list"},
    {OperationKind::LIST_DESTRUCTURE, "LIST_DESTRUCTURE", kVariadicArity, "list"},
    {OperationKind::LIST_VALIDATE, "LIST_VALIDATE", 3, "list"},
    {OperationKind::LIST_ASSERT, "LIST_ASSERT", 2, "list"},

    // MAPS
    {OperationKind::MAP_NEW, "MAP_NEW", 1, "map"},
    {OperationKind::MAP_SET, "MAP_SET", 3, "map"},
    {OperationKind::MAP_GET, "MAP_GET", 3, "map"},
    {OperationKind::MAP_HAS, "MAP_HAS", 3, "map"},
    {OperationKind::MAP_DELETE, "MAP_DELETE", 2, "map"},
    {OperationKind::MAP_KEYS, "MAP_KEYS", 2, "map"},
    {OperationKind::MAP_VALUES, "MAP_VALUES", 2, "map"},
    {OperationKind::MAP_MERGE, "MAP_MERGE", 3, "map"},
    {OperationKind::MAP_ENTRIES, "MAP_ENTRIES", 2, "map"},
    {OperationKind::MAP_VALIDATE, "MAP_VALIDATE", 4, "map"},

    // SETS
    {OperationKind::SET_NEW, "SET_NEW", 1, "set"},
    {OperationKind::SET_ADD, "SET_ADD", 2, "set"},
    {OperationKind::SET_HAS, "SET_HAS", 3, "set"},
    {OperationKind::SET_DELETE, "SET_DELETE", 2, "set"},
    {OperationKind::SET_UNION, "SET_UNION", 3, "set"},
    {OperationKind::SET_INTERSECT, "SET_INTERSECT", 3, "set"},
    {OperationKind::SET_DIFFERENCE, "SET_DIFFERENCE", 3, "set"},
    {OperationKind::SET_SYMMETRIC_DIFF, "SET_SYMMETRIC_DIFF", 3, "set"},
    {OperationKind::SET_VALIDATE, "SET_VALIDATE", 3, "set"},
    {OperationKind::SET_ASSERT, "SET_ASSERT", 2, "set"},

    // STRUCTURES
    {OperationKind::STRUCT_DEF_NEW, "STRUCT_DEF_NEW", 1, "struct"},
    {OperationKind::STRUCT_DEF_NAME, "STRUCT_DEF_NAME", 2, "struct"},
    {OperationKind::STRUCT_DEF_FIELD, "STRUCT_DEF_FIELD", 3, "struct"},
    {OperationKind::STRUCT_DEF_FIELD_DEFAULT, "STRUCT_DEF_FIELD_DEFAULT", 4, "struct"},
    {OperationKind::STRUCT_DEF_FIELD_VISIBILITY, "STRUCT_DEF_FIELD_VISIBILITY", 3, "struct"},
    {OperationKind::STRUCT_DEF_FIELD_IMMUTABLE, "STRUCT_DEF_FIELD_IMMUTABLE", 3, "struct"},
    {OperationKind::STRUCT_DEF_METHOD, "STRUCT_DEF_METHOD", 3, "struct"},
    {OperationKind::STRUCT_DEF_VALIDATOR, "STRUCT_DEF_VALIDATOR", 2, "struct"},
    {OperationKind::STRUCT_DEF_IMPLEMENT, "STRUCT_DEF_IMPLEMENT", 2, "struct"},
    {OperationKind::STRUCT_DEF_EXTEND, "STRUCT_DEF_EXTEND", 2, "struct"},
    {OperationKind::STRUCT_DEF_SEAL, "STRUCT_DEF_SEAL", 1, "struct"},
    {OperationKind::STRUCT_NEW, "STRUCT_NEW", 2, "struct"},
    {OperationKind::STRUCT_INIT, "STRUCT_INIT", kVariadicArity, "struct"},
    {OperationKind::STRUCT_GET, "STRUCT_GET", 3, "struct"},
    {OperationKind::STRUCT_SET, "STRUCT_SET", 3, "struct"},
    {OperationKind::STRUCT_GET_I, "STRUCT_GET_I", 3, "struct"},
    {OperationKind::STRUCT_SET_I, "STRUCT_SET_I", 3, "struct"},
    {OperationKind::STRUCT_TYPEOF, "STRUCT_TYPEOF", 2, "struct"},
    {OperationKind::STRUCT_IS, "STRUCT_IS", 3, "struct"},
    {OperationKind::STRUCT_COPY, "STRUCT_COPY", 2, "struct"},
    {OperationKind::STRUCT_EQ, "STRUCT_EQ", 3, "struct"},
    {OperationKind::STRUCT_CALL, "STRUCT_CALL", 3, "struct"},
    {OperationKind::STRUCT_FIELDS, "STRUCT_FIELDS", 2, "struct"},
    {OperationKind::STRUCT_FIELD_INFO, "STRUCT_FIELD_INFO", 3, "struct"},
    {OperationKind::STRUCT_METHODS, "STRUCT_METHODS", 2, "struct"},
    {OperationKind::STRUCT_INTERFACES, "STRUCT_INTERFACES", 2, "struct"},
    {OperationKind::STRUCT_IMPLEMENTS, "STRUCT_IMPLEMENTS", 3, "struct"},
    {OperationKind::STRUCT_COMPOSE, "STRUCT_COMPOSE", 3, "struct"},
    {OperationKind::STRUCT_DESTRUCTURE, "STRUCT_DESTRUCTURE", kVariadicArity, "struct"},

    // INTERFACES
    {OperationKind::INTERFACE_NEW, "INTERFACE_NEW", 1, "interface"},
    {OperationKind::INTERFACE_NAME, "INTERFACE_NAME", 2, "interface"},
    {OperationKind::INTERFACE_METHOD, "INTERFACE_METHOD", 2, "interface"},
    {OperationKind::INTERFACE_HAS, "INTERFACE_HAS", 3, "interface"},

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
    {OperationKind::VEC_PROJECT, "VEC_PROJECT", 3, "vector"},
    {OperationKind::VEC_REFLECT, "VEC_REFLECT", 3, "vector"},
    {OperationKind::VEC_ANGLE, "VEC_ANGLE", 3, "vector"},
    {OperationKind::VEC_DISTANCE, "VEC_DISTANCE", 3, "vector"},
    {OperationKind::VEC_LERP, "VEC_LERP", 3, "vector"},

    // MATRIX
    {OperationKind::MAT_NEW, "MAT_NEW", 3, "matrix"},
    {OperationKind::MAT_GET, "MAT_GET", 3, "matrix"},
    {OperationKind::MAT_SET, "MAT_SET", 3, "matrix"},
    {OperationKind::MAT_ROWS, "MAT_ROWS", 2, "matrix"},
    {OperationKind::MAT_COLS, "MAT_COLS", 2, "matrix"},
    {OperationKind::MAT_MUL, "MAT_MUL", 3, "matrix"},
    {OperationKind::MAT_ADD, "MAT_ADD", 3, "matrix"},
    {OperationKind::MAT_SUB, "MAT_SUB", 3, "matrix"},
    {OperationKind::MAT_SCALE, "MAT_SCALE", 3, "matrix"},
    {OperationKind::MAT_IDENTITY, "MAT_IDENTITY", 2, "matrix"},
    {OperationKind::MAT_TRACE, "MAT_TRACE", 2, "matrix"},
    {OperationKind::MAT_RREF, "MAT_RREF", 2, "matrix"},
    {OperationKind::MAT_RANK, "MAT_RANK", 2, "matrix"},
    {OperationKind::MAT_EIGEN, "MAT_EIGEN", 2, "matrix"},
    {OperationKind::MAT_SOLVE, "MAT_SOLVE", 3, "matrix"},
    {OperationKind::MAT_TRANSPOSE, "MAT_TRANSPOSE", 2, "matrix"},
    {OperationKind::MAT_DET, "MAT_DET", 2, "matrix"},
    {OperationKind::MAT_INV, "MAT_INV", 2, "matrix"},

    // STRINGS
    {OperationKind::STR_CONCAT, "STR_CONCAT", 3, "string"},
    {OperationKind::STR_LEN, "STR_LEN", 2, "string"},
    {OperationKind::STR_SUBSTR, "STR_SUBSTR", 3, "string"},
    {OperationKind::STR_FIND, "STR_FIND", 3, "string"},
    {OperationKind::STR_REPLACE, "STR_REPLACE", 3, "string"},
    {OperationKind::STR_SPLIT, "STR_SPLIT", 3, "string"},
    {OperationKind::STR_JOIN, "STR_JOIN", 3, "string"},
    {OperationKind::STR_UPPER, "STR_UPPER", 2, "string"},
    {OperationKind::STR_LOWER, "STR_LOWER", 2, "string"},
    {OperationKind::STR_TRIM, "STR_TRIM", 2, "string"},
    {OperationKind::STR_EQ, "STR_EQ", 3, "string"},

    // TYPE/CAST
    {OperationKind::TYPEOF, "TYPEOF", 2, "type"},
    {OperationKind::TYPE_IS, "TYPE_IS", 3, "type"},
    {OperationKind::TYPE_ASSERT, "TYPE_ASSERT", 2, "type"},
    {OperationKind::TYPE_HINT, "TYPE_HINT", 2, "type"},
    {OperationKind::TYPE_ALIAS, "TYPE_ALIAS", 2, "type"},
    {OperationKind::CAST_INT, "CAST_INT", 2, "type"},
    {OperationKind::CAST_FLOAT, "CAST_FLOAT", 2, "type"},
    {OperationKind::CAST_BOOL, "CAST_BOOL", 2, "type"},
    {OperationKind::CAST_STRING, "CAST_STRING", 2, "type"},
    {OperationKind::IS_NULL, "IS_NULL", 2, "type"},
    {OperationKind::IS_NAN, "IS_NAN", 2, "type"},
    {OperationKind::IS_INF, "IS_INF", 2, "type"},

    // FILE/TIME
    {OperationKind::READ_FILE, "READ_FILE", 2, "io"},
    {OperationKind::WRITE_FILE, "WRITE_FILE", 2, "io"},
    {OperationKind::APPEND_FILE, "APPEND_FILE", 2, "io"},
    {OperationKind::FILE_EXISTS, "FILE_EXISTS", 2, "io"},
    {OperationKind::DELETE_FILE, "DELETE_FILE", 1, "io"},
    {OperationKind::TIME_NOW, "TIME_NOW", 1, "time"},
    {OperationKind::SLEEP, "SLEEP", 1, "time"},

    // BRANCHING
    {OperationKind::JMP_IF, "JMP_IF", 2, "control_flow"},
    {OperationKind::JMP_IF_ZERO, "JMP_IF_ZERO", 2, "control_flow"},
    {OperationKind::JMP_IF_NOT, "JMP_IF_NOT", 2, "control_flow"},
    {OperationKind::JMP_EQ, "JMP_EQ", 3, "control_flow"},
    {OperationKind::JMP_NEQ, "JMP_NEQ", 3, "control_flow"},
    {OperationKind::JMP_LT, "JMP_LT", 3, "control_flow"},
    {OperationKind::JMP_LTE, "JMP_LTE", 3, "control_flow"},
    {OperationKind::JMP_GT, "JMP_GT", 3, "control_flow"},
    {OperationKind::JMP_GTE, "JMP_GTE", 3, "control_flow"},

    // MEMORY/STACK
    {OperationKind::LOAD, "LOAD", 2, "memory"},
    {OperationKind::STORE, "STORE", 2, "memory"},
    {OperationKind::SWAP, "SWAP", 2, "memory"},
    {OperationKind::CLEAR, "CLEAR", 1, "memory"},

    // EXTRA MATH
    {OperationKind::SIGN, "SIGN", 2, "math"},
    {OperationKind::CBRT, "CBRT", 2, "math"},
    {OperationKind::FACTORIAL, "FACTORIAL", 2, "math"},
    {OperationKind::GCD, "GCD", 3, "math"},
    {OperationKind::LCM, "LCM", 3, "math"},
    {OperationKind::REM, "REM", 3, "math"},
    {OperationKind::FMOD, "FMOD", 3, "math"},
    {OperationKind::DEG2RAD, "DEG2RAD", 2, "math"},
    {OperationKind::RAD2DEG, "RAD2DEG", 2, "math"},
    {OperationKind::LERP, "LERP", 3, "math"},
    {OperationKind::MAP_RANGE, "MAP_RANGE", 3, "math"},

    // RANDOM
    {OperationKind::RAND_RANDOM, "RAND_RANDOM", 1, "random"},
    {OperationKind::RAND_INT, "RAND_INT", 3, "random"},
    {OperationKind::RAND_RANGE, "RAND_RANGE", 3, "random"},
});

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
            throw std::runtime_error(format_error_context(
                ErrorPhase::Parse,
                "Duplicate operation name in operation table: " + std::string(definition.name)));
        }

        const auto [__, inserted_kind] = seen_kinds.emplace(static_cast<int>(definition.kind));
        if (!inserted_kind) {
            throw std::runtime_error(format_error_context(
                ErrorPhase::Parse,
                "Duplicate operation kind for opcode: " + std::string(definition.name)));
        }

        operation_by_name.emplace(
            definition.name,
            Operation{definition.kind, definition.name, definition.arity, definition.category}
        );
    }
}

} // namespace aic
