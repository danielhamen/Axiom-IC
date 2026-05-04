#include "optimizer.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <unordered_set>
#include <vector>

namespace aic {
namespace {

void sync_operands(Instruction& ins) {
    ins.x = ins.operands.size() > 0 ? ins.operands[0] : Operand{};
    ins.y = ins.operands.size() > 1 ? ins.operands[1] : Operand{};
    ins.z = ins.operands.size() > 2 ? ins.operands[2] : Operand{};
}

bool is_same_slot(const Operand& lhs, const Operand& rhs) {
    return lhs.kind == OperandKind::Slot && rhs.kind == OperandKind::Slot && lhs.value == rhs.value;
}

bool is_unreachable_terminal(OperationKind op) {
    return op == OperationKind::HALT ||
           op == OperationKind::RET ||
           op == OperationKind::THROW;
}

bool is_label_target(OperationKind op) {
    switch (op) {
        case OperationKind::JMP:
        case OperationKind::JMP_IF:
        case OperationKind::JMP_IF_ZERO:
        case OperationKind::JMP_IF_NOT:
        case OperationKind::JMP_EQ:
        case OperationKind::JMP_NEQ:
        case OperationKind::JMP_LT:
        case OperationKind::JMP_LTE:
        case OperationKind::JMP_GT:
        case OperationKind::JMP_GTE:
        case OperationKind::TRY:
        case OperationKind::FINALLY:
            return true;
        default:
            return false;
    }
}

std::optional<Value> static_value(const Program& program, const Operand& operand) {
    if (operand.kind == OperandKind::Immediate && operand.has_immediate) {
        return operand.immediate;
    }
    if (operand.kind == OperandKind::Constant && operand.value >= 0) {
        const size_t index = static_cast<size_t>(operand.value);
        if (index < program.constants.size()) {
            return program.constants[index];
        }
    }
    return std::nullopt;
}

bool is_numeric(const Value& value) {
    return value.kind == ValueKind::Integer || value.kind == ValueKind::Float;
}

double as_double(const Value& value) {
    return value.kind == ValueKind::Integer ? static_cast<double>(value.i) : value.f;
}

Operand immediate_operand(Value value) {
    Operand out{};
    out.kind = OperandKind::Immediate;
    out.immediate = std::move(value);
    out.has_immediate = true;
    return out;
}

Instruction load_immediate(const Operand& dst, Value value) {
    Instruction out{};
    out.op = OperationKind::LOAD;
    out.operands = {dst, immediate_operand(std::move(value))};
    sync_operands(out);
    return out;
}

std::optional<Value> fold_arithmetic(const Program& program, const Instruction& ins) {
    auto y = ins.operands.size() > 1 ? static_value(program, ins.operands[1]) : std::nullopt;
    auto z = ins.operands.size() > 2 ? static_value(program, ins.operands[2]) : std::nullopt;

    auto int_or_float = [](const Value& lhs, const Value& rhs, double result, int64_t int_result) {
        Value out{};
        if (lhs.kind == ValueKind::Integer && rhs.kind == ValueKind::Integer) {
            out.kind = ValueKind::Integer;
            out.i = int_result;
        } else {
            out.kind = ValueKind::Float;
            out.f = result;
        }
        return out;
    };

    switch (ins.op) {
        case OperationKind::ADD:
            if (y && z && is_numeric(*y) && is_numeric(*z)) {
                return int_or_float(*y, *z, as_double(*y) + as_double(*z), y->i + z->i);
            }
            break;
        case OperationKind::SUB:
            if (y && z && is_numeric(*y) && is_numeric(*z)) {
                return int_or_float(*y, *z, as_double(*y) - as_double(*z), y->i - z->i);
            }
            break;
        case OperationKind::MUL:
            if (y && z && is_numeric(*y) && is_numeric(*z)) {
                return int_or_float(*y, *z, as_double(*y) * as_double(*z), y->i * z->i);
            }
            break;
        case OperationKind::DIV:
            if (y && z && is_numeric(*y) && is_numeric(*z) && as_double(*z) != 0.0) {
                const int64_t int_result = y->kind == ValueKind::Integer && z->kind == ValueKind::Integer
                                               ? y->i / z->i
                                               : 0;
                return int_or_float(*y, *z, as_double(*y) / as_double(*z), int_result);
            }
            break;
        case OperationKind::MOD:
            if (y && z && y->kind == ValueKind::Integer && z->kind == ValueKind::Integer && z->i != 0) {
                Value out{};
                out.kind = ValueKind::Integer;
                out.i = y->i % z->i;
                return out;
            }
            break;
        case OperationKind::NEG:
            if (y && is_numeric(*y)) {
                Value out{};
                if (y->kind == ValueKind::Integer) {
                    out.kind = ValueKind::Integer;
                    out.i = -y->i;
                } else {
                    out.kind = ValueKind::Float;
                    out.f = -y->f;
                }
                return out;
            }
            break;
        case OperationKind::INC:
            if (y && is_numeric(*y)) {
                Value out{};
                if (y->kind == ValueKind::Integer) {
                    out.kind = ValueKind::Integer;
                    out.i = y->i + 1;
                } else {
                    out.kind = ValueKind::Float;
                    out.f = y->f + 1.0;
                }
                return out;
            }
            break;
        case OperationKind::DEC:
            if (y && is_numeric(*y)) {
                Value out{};
                if (y->kind == ValueKind::Integer) {
                    out.kind = ValueKind::Integer;
                    out.i = y->i - 1;
                } else {
                    out.kind = ValueKind::Float;
                    out.f = y->f - 1.0;
                }
                return out;
            }
            break;
        case OperationKind::ABS:
            if (y && is_numeric(*y)) {
                Value out{};
                if (y->kind == ValueKind::Integer) {
                    out.kind = ValueKind::Integer;
                    out.i = std::abs(y->i);
                } else {
                    out.kind = ValueKind::Float;
                    out.f = std::fabs(y->f);
                }
                return out;
            }
            break;
        default:
            break;
    }

    return std::nullopt;
}

void remap_labels(Function& function, const std::vector<size_t>& old_to_new, size_t new_size) {
    for (auto& [_, pc] : function.labels) {
        if (pc >= old_to_new.size()) {
            pc = new_size;
            continue;
        }
        if (old_to_new[pc] != std::numeric_limits<size_t>::max()) {
            pc = old_to_new[pc];
            continue;
        }
        size_t next = pc + 1;
        while (next < old_to_new.size() && old_to_new[next] == std::numeric_limits<size_t>::max()) {
            next++;
        }
        pc = next < old_to_new.size() ? old_to_new[next] : new_size;
    }
}

void optimize_function(Program& program, Function& function, OptimizationStats& stats) {
    const size_t old_size = function.ins.size();
    std::vector<bool> label_boundaries(old_size, false);
    for (const auto& [_, pc] : function.labels) {
        if (pc < old_size) {
            label_boundaries[pc] = true;
        }
    }

    std::vector<Instruction> optimized;
    std::vector<size_t> old_to_new(old_size, std::numeric_limits<size_t>::max());
    bool unreachable = false;

    for (size_t pc = 0; pc < old_size; pc++) {
        Instruction ins = function.ins[pc];
        sync_operands(ins);

        if (label_boundaries[pc]) {
            unreachable = false;
        }

        if (unreachable) {
            stats.removed_unreachable++;
            continue;
        }

        if (ins.op == OperationKind::LOAD &&
            ins.operands.size() == 2 &&
            is_same_slot(ins.operands[0], ins.operands[1])) {
            stats.removed_self_loads++;
            continue;
        }

        if (ins.op == OperationKind::JMP &&
            ins.operands.size() == 1 &&
            ins.operands[0].kind == OperandKind::Label) {
            const auto target = function.labels.find(ins.operands[0].strval);
            if (target != function.labels.end() && target->second == pc + 1) {
                stats.removed_next_jumps++;
                continue;
            }
        }

        if (!ins.operands.empty()) {
            if (auto folded = fold_arithmetic(program, ins)) {
                ins = load_immediate(ins.operands[0], std::move(*folded));
                stats.folded_constant_arithmetic++;
            }
        }

        old_to_new[pc] = optimized.size();
        optimized.push_back(std::move(ins));

        if (is_unreachable_terminal(optimized.back().op)) {
            unreachable = true;
        }
    }

    function.ins = std::move(optimized);
    remap_labels(function, old_to_new, function.ins.size());

    for (Instruction& ins : function.ins) {
        sync_operands(ins);
    }

    if (stats.folded_constant_arithmetic > 0) {
        program.imported_categories.insert("memory");
    }
}

void resolve_label_operands(Function& function, OptimizationStats& stats) {
    for (Instruction& ins : function.ins) {
        if (!is_label_target(ins.op) || ins.operands.empty() || ins.operands[0].kind != OperandKind::Label) {
            continue;
        }
        auto target = function.labels.find(ins.operands[0].strval);
        if (target == function.labels.end()) {
            continue;
        }
        ins.operands[0].value = static_cast<int64_t>(target->second);
        ins.operands[0].resolved = true;
        sync_operands(ins);
        stats.resolved_label_operands++;
    }
}

void resolve_function_operand(Program& program, Instruction& ins, size_t index, OptimizationStats& stats) {
    if (index >= ins.operands.size()) {
        return;
    }
    Operand& operand = ins.operands[index];
    if (operand.kind != OperandKind::Function && operand.kind != OperandKind::Label) {
        return;
    }
    auto function_index = program.functions.try_index_of(operand.strval);
    if (!function_index.has_value()) {
        return;
    }
    operand.kind = OperandKind::Function;
    operand.value = static_cast<int64_t>(*function_index);
    operand.resolved = true;
    sync_operands(ins);
    stats.resolved_function_operands++;
}

void resolve_function_operands(Program& program, Function& function, OptimizationStats& stats) {
    for (Instruction& ins : function.ins) {
        switch (ins.op) {
            case OperationKind::CALL:
                resolve_function_operand(program, ins, 0, stats);
                break;
            case OperationKind::LIST_MAP:
            case OperationKind::LIST_FILTER:
            case OperationKind::LIST_REDUCE:
                resolve_function_operand(program, ins, 2, stats);
                break;
            default:
                break;
        }
    }
}

} // namespace

OptimizationStats optimize_program(Program& program) {
    OptimizationStats stats{};

    for (size_t i = 0; i < program.functions.size(); i++) {
        Function* function = program.functions.at(i);
        optimize_function(program, *function, stats);
    }

    for (size_t i = 0; i < program.functions.size(); i++) {
        Function* function = program.functions.at(i);
        resolve_label_operands(*function, stats);
        resolve_function_operands(program, *function, stats);
    }

    return stats;
}

} // namespace aic
