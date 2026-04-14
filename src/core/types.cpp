#include "types.hpp"

namespace aic {

std::unordered_map<std::string, OperandKind> operand_kind_map = {
    {"None", OperandKind::None},
    {"Slot", OperandKind::Slot},
    {"Immediate", OperandKind::Immediate},
    {"Constant", OperandKind::Constant},
    {"Label", OperandKind::Label},
    {"Address", OperandKind::Address},
};

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

} // namespace aic
