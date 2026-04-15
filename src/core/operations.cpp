#include "operations.hpp"

#include <array>
#include <stdexcept>
#include <unordered_set>

namespace aic {

namespace {

constexpr std::array<OperationDefinition, 18> kOperationTable{{
    {OperationKind::ADD, "ADD", 3, "arithmetic"},
    {OperationKind::SUB, "SUB", 3, "arithmetic"},
    {OperationKind::MUL, "MUL", 3, "arithmetic"},
    {OperationKind::DIV, "DIV", 3, "arithmetic"},
    {OperationKind::MOD, "MOD", 3, "arithmetic"},
    {OperationKind::JMP, "JMP", 1, "control_flow"},
    {OperationKind::HALT, "HALT", 0, "control_flow"},
    {OperationKind::PRINT, "PRINT", 1, "io"},
    {OperationKind::PUSH, "PUSH", 1, "stack"},
    {OperationKind::POP, "POP", 1, "stack"},
    {OperationKind::CALL, "CALL", 1, "function"},
    {OperationKind::RET, "RET", 0, "function"},
    {OperationKind::RETVAL, "RETVAL", 1, "function"},
    {OperationKind::LIST_NEW, "LIST_NEW", 2, "list"},
    {OperationKind::LIST_PUSH, "LIST_PUSH", 2, "list"},
    {OperationKind::LIST_GET, "LIST_GET", 3, "list"},
    {OperationKind::LIST_SET, "LIST_SET", 3, "list"},
    {OperationKind::LIST_LEN, "LIST_LEN", 2, "list"},
}};

} // namespace

std::unordered_map<std::string, Operation> operation_by_name;

std::span<const OperationDefinition> operation_definitions() {
    return kOperationTable;
}

void register_operations() {
    if (!operation_by_name.empty()) {
        return;
    }

    operation_by_name.reserve(kOperationTable.size());

    std::unordered_set<std::string> seen_names;
    std::unordered_set<int> seen_kinds;
    seen_names.reserve(kOperationTable.size());
    seen_kinds.reserve(kOperationTable.size());

    for (const auto& definition : kOperationTable) {
        const auto [_, inserted_name] = seen_names.emplace(definition.name);
        if (!inserted_name) {
            throw std::runtime_error("Duplicate operation name: " + std::string(definition.name));
        }

        const auto [__, inserted_kind] = seen_kinds.emplace(static_cast<int>(definition.kind));
        if (!inserted_kind) {
            throw std::runtime_error(
                "Duplicate operation kind for opcode: " + std::string(definition.name)
            );
        }

        operation_by_name.emplace(
            definition.name,
            Operation{definition.kind, definition.name, definition.arity}
        );
    }
}

} // namespace aic
