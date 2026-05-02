#include "types.hpp"

#include <sstream>
#include <format>

namespace aic {

std::unordered_map<std::string, OperandKind> operand_kind_map = {
    {"None", OperandKind::None},
    {"Slot", OperandKind::Slot},
    {"Immediate", OperandKind::Immediate},
    {"Constant", OperandKind::Constant},
    {"Label", OperandKind::Label},
    {"Address", OperandKind::Address},
    {"Function", OperandKind::Function},
    {"Argument", OperandKind::Argument},
};

std::string operation_kind_to_string(OperationKind kind) {
    switch (kind) {
        case OperationKind::ADD:
            return "ADD";
        case OperationKind::SUB:
            return "SUB";
        case OperationKind::MUL:
            return "MUL";
        case OperationKind::DIV:
            return "DIV";
        case OperationKind::MOD:
            return "MOD";
        case OperationKind::NEG:
            return "NEG";
        case OperationKind::INC:
            return "INC";
        case OperationKind::DEC:
            return "DEC";
        case OperationKind::ABS:
            return "ABS";
        case OperationKind::EQ:
            return "EQ";
        case OperationKind::NEQ:
            return "NEQ";
        case OperationKind::LT:
            return "LT";
        case OperationKind::LTE:
            return "LTE";
        case OperationKind::GT:
            return "GT";
        case OperationKind::GTE:
            return "GTE";
        case OperationKind::AND:
            return "AND";
        case OperationKind::OR:
            return "OR";
        case OperationKind::NOT:
            return "NOT";
        case OperationKind::XOR:
            return "XOR";
        case OperationKind::POW:
            return "POW";
        case OperationKind::SQRT:
            return "SQRT";
        case OperationKind::EXP:
            return "EXP";
        case OperationKind::LOG:
            return "LOG";
        case OperationKind::LOG2:
            return "LOG2";
        case OperationKind::LOG10:
            return "LOG10";
        case OperationKind::LN:
            return "LN";
        case OperationKind::SIN:
            return "SIN";
        case OperationKind::COS:
            return "COS";
        case OperationKind::TAN:
            return "TAN";
        case OperationKind::CSC:
            return "CSC";
        case OperationKind::SEC:
            return "SEC";
        case OperationKind::COT:
            return "COT";
        case OperationKind::ASIN:
            return "ASIN";
        case OperationKind::ACOS:
            return "ACOS";
        case OperationKind::ATAN:
            return "ATAN";
        case OperationKind::ACSC:
            return "ACSC";
        case OperationKind::ASEC:
            return "ASEC";
        case OperationKind::ACOT:
            return "ACOT";
        case OperationKind::SINH:
            return "SINH";
        case OperationKind::COSH:
            return "COSH";
        case OperationKind::TANH:
            return "TANH";
        case OperationKind::CSCH:
            return "CSCH";
        case OperationKind::SECH:
            return "SECH";
        case OperationKind::COTH:
            return "COTH";
        case OperationKind::ASINH:
            return "ASINH";
        case OperationKind::ACOSH:
            return "ACOSH";
        case OperationKind::ATANH:
            return "ATANH";
        case OperationKind::ACSCH:
            return "ACSCH";
        case OperationKind::ASECH:
            return "ASECH";
        case OperationKind::ACOTH:
            return "ACOTH";
        case OperationKind::ATAN2:
            return "ATAN2";
        case OperationKind::HYPOT:
            return "HYPOT";
        case OperationKind::FLOOR:
            return "FLOOR";
        case OperationKind::CEIL:
            return "CEIL";
        case OperationKind::ROUND:
            return "ROUND";
        case OperationKind::MIN:
            return "MIN";
        case OperationKind::MAX:
            return "MAX";
        case OperationKind::CLAMP:
            return "CLAMP";
        case OperationKind::HALT:
            return "HALT";
        case OperationKind::JMP:
            return "JMP";
        case OperationKind::PRINT:
            return "PRINT";
        case aic::OperationKind::PRINTLN:
            return "PRINTLN";
        case OperationKind::PUSH:
            return "PUSH";
        case OperationKind::POP:
            return "POP";
        case OperationKind::CALL:
            return "CALL";
        case OperationKind::RET:
            return "RET";
        case OperationKind::RETVAL:
            return "RETVAL";
        case OperationKind::LIST_NEW:
            return "LIST_NEW";
        case OperationKind::LIST_PUSH:
            return "LIST_PUSH";
        case OperationKind::LIST_POP:
            return "LIST_POP";
        case OperationKind::LIST_INSERT:
            return "LIST_INSERT";
        case OperationKind::LIST_ERASE:
            return "LIST_ERASE";
        case OperationKind::LIST_SLICE:
            return "LIST_SLICE";
        case OperationKind::LIST_CLEAR:
            return "LIST_CLEAR";
        case OperationKind::LIST_FIND:
            return "LIST_FIND";
        case OperationKind::LIST_SORT:
            return "LIST_SORT";
        case OperationKind::LIST_REVERSE:
            return "LIST_REVERSE";
        case OperationKind::LIST_GET:
            return "LIST_GET";
        case OperationKind::LIST_SET:
            return "LIST_SET";
        case OperationKind::LIST_LEN:
            return "LIST_LEN";
        case OperationKind::MAP_NEW:
            return "MAP_NEW";
        case OperationKind::MAP_SET:
            return "MAP_SET";
        case OperationKind::MAP_GET:
            return "MAP_GET";
        case OperationKind::MAP_HAS:
            return "MAP_HAS";
        case OperationKind::MAP_DELETE:
            return "MAP_DELETE";
        case OperationKind::MAP_KEYS:
            return "MAP_KEYS";
        case OperationKind::MAP_VALUES:
            return "MAP_VALUES";
        case OperationKind::SET_NEW:
            return "SET_NEW";
        case OperationKind::SET_ADD:
            return "SET_ADD";
        case OperationKind::SET_HAS:
            return "SET_HAS";
        case OperationKind::SET_DELETE:
            return "SET_DELETE";
        case OperationKind::SET_UNION:
            return "SET_UNION";
        case OperationKind::SET_INTERSECT:
            return "SET_INTERSECT";
        case OperationKind::VEC_NEW:
            return "VEC_NEW";
        case OperationKind::VEC_PUSH:
            return "VEC_PUSH";
        case OperationKind::VEC_GET:
            return "VEC_GET";
        case OperationKind::VEC_SET:
            return "VEC_SET";
        case OperationKind::VEC_LEN:
            return "VEC_LEN";
        case OperationKind::VEC_ADD:
            return "VEC_ADD";
        case OperationKind::VEC_SUB:
            return "VEC_SUB";
        case OperationKind::VEC_SCALE:
            return "VEC_SCALE";
        case OperationKind::VEC_DOT:
            return "VEC_DOT";
        case OperationKind::VEC_CROSS:
            return "VEC_CROSS";
        case OperationKind::VEC_MAG:
            return "VEC_MAG";
        case OperationKind::VEC_NORM:
            return "VEC_NORM";
        case OperationKind::VEC_PROJECT:
            return "VEC_PROJECT";
        case OperationKind::VEC_REFLECT:
            return "VEC_REFLECT";
        case OperationKind::VEC_ANGLE:
            return "VEC_ANGLE";
        case OperationKind::VEC_DISTANCE:
            return "VEC_DISTANCE";
        case OperationKind::VEC_LERP:
            return "VEC_LERP";
        case OperationKind::MAT_NEW:
            return "MAT_NEW";
        case OperationKind::MAT_GET:
            return "MAT_GET";
        case OperationKind::MAT_SET:
            return "MAT_SET";
        case OperationKind::MAT_ROWS:
            return "MAT_ROWS";
        case OperationKind::MAT_COLS:
            return "MAT_COLS";
        case OperationKind::MAT_MUL:
            return "MAT_MUL";
        case OperationKind::MAT_ADD:
            return "MAT_ADD";
        case OperationKind::MAT_SUB:
            return "MAT_SUB";
        case OperationKind::MAT_SCALE:
            return "MAT_SCALE";
        case OperationKind::MAT_IDENTITY:
            return "MAT_IDENTITY";
        case OperationKind::MAT_TRACE:
            return "MAT_TRACE";
        case OperationKind::MAT_RREF:
            return "MAT_RREF";
        case OperationKind::MAT_RANK:
            return "MAT_RANK";
        case OperationKind::MAT_EIGEN:
            return "MAT_EIGEN";
        case OperationKind::MAT_SOLVE:
            return "MAT_SOLVE";
        case OperationKind::MAT_TRANSPOSE:
            return "MAT_TRANSPOSE";
        case OperationKind::MAT_DET:
            return "MAT_DET";
        case OperationKind::MAT_INV:
            return "MAT_INV";
        case OperationKind::STR_CONCAT:
            return "STR_CONCAT";
        case OperationKind::STR_LEN:
            return "STR_LEN";
        case OperationKind::STR_SUBSTR:
            return "STR_SUBSTR";
        case OperationKind::STR_FIND:
            return "STR_FIND";
        case OperationKind::STR_REPLACE:
            return "STR_REPLACE";
        case OperationKind::STR_SPLIT:
            return "STR_SPLIT";
        case OperationKind::STR_JOIN:
            return "STR_JOIN";
        case OperationKind::STR_UPPER:
            return "STR_UPPER";
        case OperationKind::STR_LOWER:
            return "STR_LOWER";
        case OperationKind::STR_TRIM:
            return "STR_TRIM";
        case OperationKind::STR_EQ:
            return "STR_EQ";
        case OperationKind::TYPEOF:
            return "TYPEOF";
        case OperationKind::CAST_INT:
            return "CAST_INT";
        case OperationKind::CAST_FLOAT:
            return "CAST_FLOAT";
        case OperationKind::CAST_BOOL:
            return "CAST_BOOL";
        case OperationKind::CAST_STRING:
            return "CAST_STRING";
        case OperationKind::IS_NULL:
            return "IS_NULL";
        case OperationKind::IS_NAN:
            return "IS_NAN";
        case OperationKind::IS_INF:
            return "IS_INF";
        case OperationKind::READ_FILE:
            return "READ_FILE";
        case OperationKind::WRITE_FILE:
            return "WRITE_FILE";
        case OperationKind::APPEND_FILE:
            return "APPEND_FILE";
        case OperationKind::FILE_EXISTS:
            return "FILE_EXISTS";
        case OperationKind::DELETE_FILE:
            return "DELETE_FILE";
        case OperationKind::TIME_NOW:
            return "TIME_NOW";
        case OperationKind::SLEEP:
            return "SLEEP";
        case OperationKind::JMP_IF:
            return "JMP_IF";
        case OperationKind::JMP_IF_ZERO:
            return "JMP_IF_ZERO";
        case OperationKind::JMP_IF_NOT:
            return "JMP_IF_NOT";
        case OperationKind::JMP_EQ:
            return "JMP_EQ";
        case OperationKind::JMP_NEQ:
            return "JMP_NEQ";
        case OperationKind::JMP_LT:
            return "JMP_LT";
        case OperationKind::JMP_LTE:
            return "JMP_LTE";
        case OperationKind::JMP_GT:
            return "JMP_GT";
        case OperationKind::JMP_GTE:
            return "JMP_GTE";
        case OperationKind::COPY:
            return "COPY";
        case OperationKind::STORE:
            return "STORE";
        case OperationKind::SWAP:
            return "SWAP";
        case OperationKind::CLEAR:
            return "CLEAR";
        case OperationKind::SIGN:
            return "SIGN";
        case OperationKind::CBRT:
            return "CBRT";
        case OperationKind::FACTORIAL:
            return "FACTORIAL";
        case OperationKind::GCD:
            return "GCD";
        case OperationKind::LCM:
            return "LCM";
        case OperationKind::REM:
            return "REM";
        case OperationKind::FMOD:
            return "FMOD";
        case OperationKind::DEG2RAD:
            return "DEG2RAD";
        case OperationKind::RAD2DEG:
            return "RAD2DEG";
        case OperationKind::LERP:
            return "LERP";
        case OperationKind::MAP_RANGE:
            return "MAP_RANGE";
        case OperationKind::RAND_RANDOM:
            return "RAND_RANDOM";
        case OperationKind::RAND_RANGE:
            return "RAND_RANGE";
        case OperationKind::RAND_INT:
            return "RAND_INT";
    }

    throw std::runtime_error("Unmatched OperationKind");
}

std::string value_kind_to_string(ValueKind kind) {
    switch (kind) {
        case ValueKind::Integer:
            return "Integer";
        case ValueKind::Float:
            return "Float";
        case ValueKind::String:
            return "String";
        case ValueKind::Boolean:
            return "Boolean";
        case ValueKind::Null:
            return "Null";
        case ValueKind::List:
            return "List";
        case ValueKind::Map:
            return "Map";
        case ValueKind::Set:
            return "Set";
        case ValueKind::Vector:
            return "Vector";
        case ValueKind::Matrix:
            return "Matrix";
    }

    throw std::runtime_error("Unmatched ValueKind");
}

std::string error_phase_to_string(ErrorPhase phase) {
    switch (phase) {
        case ErrorPhase::Tokenize:
            return "tokenize";
        case ErrorPhase::Parse:
            return "parse";
        case ErrorPhase::Exec:
            return "exec";
    }

    throw std::runtime_error("Unmatched ErrorPhase");
}

std::string format_error_context(ErrorPhase phase,
                                 const std::string& message,
                                 int line,
                                 int column,
                                 const std::string& function_name,
                                 const std::string& opcode,
                                 size_t pc) {
    std::ostringstream out;
    out << "[" << error_phase_to_string(phase) << "] " << message;

    if (line >= 0 && column >= 0) {
        out << " (line " << line << ", column " << column << ")";
    }

    if (!function_name.empty()) {
        out << " (fn=" << function_name << ")";
    }

    if (!opcode.empty()) {
        out << " (op=" << opcode << ")";
    }

    if (pc != static_cast<size_t>(-1)) {
        out << " (pc=" << pc << ")";
    }

    return out.str();
}

char escape_char(char in) {
    switch (in) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '0':
            return '\0';
        default:
            return in;
    }
}

bool FunctionList::exists(const std::string& fn_name) const {
    return function_indices.contains(fn_name);
}

Function* FunctionList::find(const std::string& fn_name) {
    const auto it = function_indices.find(fn_name);
    if (it == function_indices.end()) {
        return nullptr;
    }
    return &functions[it->second];
}

const Function* FunctionList::find(const std::string& fn_name) const {
    const auto it = function_indices.find(fn_name);
    if (it == function_indices.end()) {
        return nullptr;
    }
    return &functions[it->second];
}

Function* FunctionList::at(size_t idx) {
    if (idx >= functions.size()) {
        throw std::runtime_error("Error referencing function at index " + std::to_string(idx) + " as it exceeds the size of the function map");
    }

    return &functions[idx];
}

const Function* FunctionList::at(size_t idx) const {
    if (idx >= functions.size()) {
        throw std::runtime_error("Error referencing function at index " + std::to_string(idx) + " as it exceeds the size of the function map");
    }

    return &functions[idx];
}

std::optional<size_t> FunctionList::try_index_of(const std::string& fn_name) const {
    const auto it = function_indices.find(fn_name);
    if (it == function_indices.end()) {
        return std::nullopt;
    }
    return it->second;
}

size_t FunctionList::index_of(const std::string& fn_name) const {
    const auto index = try_index_of(fn_name);
    if (index.has_value()) {
        return *index;
    }
    throw std::runtime_error("Error indexing function that does not exist");
}

void FunctionList::insert(const Function& fn) {
    if (function_indices.contains(fn.name)) {
        throw std::runtime_error("Duplicate function declaration: " + fn.name);
    }
    const size_t index = functions.size();
    functions.push_back(fn);
    function_indices.emplace(fn.name, index);
}

void FunctionList::insert(Function&& fn) {
    const std::string name = fn.name;
    if (function_indices.contains(name)) {
        throw std::runtime_error("Duplicate function declaration: " + name);
    }
    const size_t index = functions.size();
    functions.push_back(std::move(fn));
    function_indices.emplace(name, index);
}

size_t FunctionList::size() const {
    return functions.size();
}

std::string Operand::to_str() const {
    if (kind == OperandKind::Immediate && has_immediate) {
        return std::format("Operand(kind={}, immediate={})", kindstr(), immediate.to_str());
    }

    if (kind == OperandKind::Label || kind == OperandKind::Function) {
        return std::format("Operand(kind={}, value={})", kindstr(), strval);
    }

    return std::format("Operand(kind={}, value={})", kindstr(), value);
}

std::string Instruction::to_string() const {
    return std::format("Instruction(op={}, x={}, y={}, z={})",
                       operation_kind_to_string(op),
                       x.to_str(),
                       y.to_str(),
                       z.to_str());
}

} // namespace aic
