#pragma once

#include "program.hpp"

#include <cstddef>

namespace aic {

struct OptimizationStats {
    size_t removed_self_loads = 0;
    size_t removed_next_jumps = 0;
    size_t folded_constant_arithmetic = 0;
    size_t removed_unreachable = 0;
    size_t folded_list_fill = 0;
    size_t folded_load_range = 0;
    size_t resolved_label_operands = 0;
    size_t resolved_function_operands = 0;
};

OptimizationStats optimize_program(Program& program);

} // namespace aic
