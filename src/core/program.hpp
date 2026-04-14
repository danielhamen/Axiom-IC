#pragma once

#include "types.hpp"

#include <vector>

namespace aic {

struct Program {
    size_t pc;
    size_t fc;
    bool halted = false;
    FunctionList functions;
    std::vector<Value> constants;
    std::vector<Value> memory;

    Value& slot(size_t index);
    void resolve(const Instruction& ins);
    void exec();
    Value read_operand(const Operand& op);
    Value read_operand_strict(const Operand& op, ValueKind enforced_type);
    void write_operand(const Operand& op, const Value& v);
};

} // namespace aic
