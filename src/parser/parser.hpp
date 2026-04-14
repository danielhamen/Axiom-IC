#pragma once

#include "../core/program.hpp"

#include <string>
#include <vector>

namespace aic {

Program parse(std::vector<std::vector<std::string>> tokens);

} // namespace aic
