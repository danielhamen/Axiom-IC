#include "types.hpp"

#include <sstream>

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
        case OperationKind::HALT:
            return "HALT";
        case OperationKind::JMP:
            return "JMP";
        case OperationKind::PRINT:
            return "PRINT";
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
        case OperationKind::LIST_GET:
            return "LIST_GET";
        case OperationKind::LIST_SET:
            return "LIST_SET";
        case OperationKind::LIST_LEN:
            return "LIST_LEN";
        case OperationKind::INT_LIST_NEW:
            return "INT_LIST_NEW";
        case OperationKind::INT_LIST_PUSH:
            return "INT_LIST_PUSH";
        case OperationKind::INT_LIST_GET:
            return "INT_LIST_GET";
        case OperationKind::INT_LIST_SET:
            return "INT_LIST_SET";
        case OperationKind::INT_LIST_LEN:
            return "INT_LIST_LEN";
        case OperationKind::FLOAT_LIST_NEW:
            return "FLOAT_LIST_NEW";
        case OperationKind::FLOAT_LIST_PUSH:
            return "FLOAT_LIST_PUSH";
        case OperationKind::FLOAT_LIST_GET:
            return "FLOAT_LIST_GET";
        case OperationKind::FLOAT_LIST_SET:
            return "FLOAT_LIST_SET";
        case OperationKind::FLOAT_LIST_LEN:
            return "FLOAT_LIST_LEN";
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
        case ValueKind::IntegerList:
            return "IntegerList";
        case ValueKind::FloatList:
            return "FloatList";
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

bool FunctionList::exists(std::string fn_name) {
    for (auto& fn : functions) {
        if (fn_name == fn.name) {
            return true;
        }
    }

    return false;
}

Function* FunctionList::find(std::string fn_name) {
    for (auto& fn : functions) {
        if (fn_name == fn.name) {
            return &fn;
        }
    }

    return nullptr;
}

Function* FunctionList::at(size_t idx) {
    if (idx >= functions.size()) {
        throw std::runtime_error("Error referencing function at index " + std::to_string(idx) + " as it exceeds the size of the function map");
    }

    return &functions.at(idx);
}

size_t FunctionList::index_of(std::string fn_name) {
    for (int i = 0; i < functions.size(); i++) {
        auto& fn = functions.at(i);
        if (fn_name == fn.name) {
            return i;
        }
    }

    throw std::runtime_error("Error indexing function that does not exist");
}

void FunctionList::insert(Function& fn) {
    functions.push_back(fn);
}

const size_t FunctionList::size() {
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
