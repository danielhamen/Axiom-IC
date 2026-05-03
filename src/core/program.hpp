#pragma once

#include "types.hpp"

#include <chrono>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace aic {

struct CallFrame {
    size_t return_pc = 0;
    size_t return_fc = 0;
    std::vector<Value> args;
    std::unordered_map<std::string, Value> kwargs;
    std::vector<Value> memory;
    std::vector<std::string> slot_type_hints;
    Value return_value{};
    bool has_return_value = false;
};

struct ExceptionHandler {
    size_t function_index = 0;
    size_t catch_pc = 0;
    size_t finally_pc = static_cast<size_t>(-1);
    size_t call_stack_depth = 0;
};

struct DiagnosticWarning {
    std::string message;
    std::string function_name;
    std::string opcode;
    size_t pc = static_cast<size_t>(-1);
};

struct Program {
    size_t pc;
    size_t fc;
    bool halted = false;
    FunctionList functions;
    std::vector<Value> constants;
    // Root frame memory for .main. Called functions store slots in their CallFrame.
    std::vector<Value> memory;
    std::vector<std::string> slot_type_hints;
    std::vector<Value> stack;
    std::vector<Value> pending_args;
    std::unordered_map<std::string, Value> pending_kwargs;
    std::unordered_set<std::string> imported_categories;
    std::unordered_map<std::string, Operand> literal_aliases;
    std::unordered_map<std::string, std::string> type_aliases;
    std::vector<CallFrame> call_stack;
    std::vector<ExceptionHandler> exception_handlers;
    Value current_error{};
    bool has_current_error = false;
    std::vector<DiagnosticWarning> warnings;
    bool warnings_enabled = true;
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
    void warn(const std::string& message, const Instruction* ins = nullptr);
};

} // namespace aic
