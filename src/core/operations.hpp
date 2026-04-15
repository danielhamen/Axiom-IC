#pragma once

#include "types.hpp"

#include <span>
#include <string>
#include <unordered_map>

namespace aic {

struct OperationDefinition {
    OperationKind kind;
    const char* name;
    size_t arity;
    const char* category;
};

extern std::unordered_map<std::string, Operation> operation_by_name;

std::span<const OperationDefinition> operation_definitions();
void register_operations();

} // namespace aic
