#pragma once

#include "types.hpp"

#include <string>
#include <unordered_map>

namespace aic {

extern std::unordered_map<std::string, Operation> operation_by_name;

void register_operations();

} // namespace aic
