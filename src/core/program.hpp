#pragma once

#include "types.hpp"

#include <chrono>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

namespace aic {

struct CallFrame {
    size_t return_pc = 0;
    size_t return_fc = 0;
    std::vector<Value> args;
    Value return_value{};
    bool has_return_value = false;
};

struct Program {
    size_t pc;
    size_t fc;
    bool halted = false;
    FunctionList functions;
    std::vector<Value> constants;
    std::vector<Value> memory;
    std::vector<Value> stack;
    std::vector<CallFrame> call_stack;
    static std::mt19937 rng;

    Value& slot(size_t index);
    void resolve(const Instruction& ins);
    struct ExecutionStats {
        std::unordered_map<OperationKind, size_t> opcode_counts;
        std::unordered_map<OperationKind, std::chrono::nanoseconds> opcode_time;
        std::unordered_map<std::string, std::chrono::nanoseconds> function_time;
        std::unordered_map<std::string, size_t> function_instruction_counts;
    };

    struct ExecutionOptions {
        bool profile = false;
        bool opcode_stats = false;
        bool trace_time = false;
        ExecutionStats* stats = nullptr;
    };

    void exec(const ExecutionOptions& options);
    void exec();
    Value read_operand(const Operand& op);
    Value read_operand_strict(const Operand& op, ValueKind enforced_type);
    void write_operand(const Operand& op, const Value& v);
};

} // namespace aic
