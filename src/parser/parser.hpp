#pragma once

#include "../core/program.hpp"
#include "tokenizer.hpp"

#include <vector>

namespace aic {

Program parse(const std::vector<Token>& tokens);

} // namespace aic
