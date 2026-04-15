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
        case OperationKind::VNEW:
            return "VNEW";
        case OperationKind::VPUSH:
            return "VPUSH";
        case OperationKind::VGET:
            return "VGET";
        case OperationKind::VSET:
            return "VSET";
        case OperationKind::VLEN:
            return "VLEN";
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
        case ValueKind::Vector:
            return "Vector";
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
