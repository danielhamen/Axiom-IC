#include "verifier.hpp"

#include "operations.hpp"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace aic {

namespace {

std::unordered_map<OperationKind, OperationDefinition> operation_metadata() {
    std::unordered_map<OperationKind, OperationDefinition> out;
    for (const OperationDefinition& definition : operation_definitions()) {
        out.emplace(definition.kind, definition);
    }
    return out;
}

const OperationDefinition* find_definition(OperationKind kind) {
    static const std::unordered_map<OperationKind, OperationDefinition> definitions = operation_metadata();
    const auto it = definitions.find(kind);
    if (it == definitions.end()) {
        return nullptr;
    }
    return &it->second;
}

bool is_readable_operand(const Operand& operand) {
    return operand.kind == OperandKind::Slot ||
           operand.kind == OperandKind::Immediate ||
           operand.kind == OperandKind::Constant ||
           operand.kind == OperandKind::Argument;
}

bool is_string_immediate(const Operand& operand) {
    return operand.kind == OperandKind::Immediate &&
           operand.has_immediate &&
           operand.immediate.kind == ValueKind::String;
}

bool is_function_name_operand(const Operand& operand) {
    return operand.kind == OperandKind::Function ||
           operand.kind == OperandKind::Label ||
           is_string_immediate(operand) ||
           operand.kind == OperandKind::Constant ||
           operand.kind == OperandKind::Slot;
}

std::string function_name_from_static_operand(const Operand& operand) {
    if (operand.kind == OperandKind::Function || operand.kind == OperandKind::Label) {
        return operand.strval;
    }
    if (is_string_immediate(operand)) {
        return operand.immediate.s;
    }
    return "";
}

bool writes_first_operand(OperationKind op) {
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
        case OperationKind::HALT:
        case OperationKind::PRINT:
        case OperationKind::PRINTLN:
        case OperationKind::PUSH:
        case OperationKind::ARG:
        case OperationKind::KWARG:
        case OperationKind::CALL:
        case OperationKind::RET:
        case OperationKind::RETVAL:
        case OperationKind::ARG_REQUIRE:
        case OperationKind::KWARG_REQUIRE:
        case OperationKind::LIST_PUSH:
        case OperationKind::LIST_INSERT:
        case OperationKind::LIST_ERASE:
        case OperationKind::LIST_CLEAR:
        case OperationKind::LIST_SORT:
        case OperationKind::LIST_REVERSE:
        case OperationKind::LIST_SET:
        case OperationKind::MAP_SET:
        case OperationKind::MAP_DELETE:
        case OperationKind::SET_ADD:
        case OperationKind::SET_DELETE:
        case OperationKind::STRUCT_DEF_NAME:
        case OperationKind::STRUCT_DEF_FIELD:
        case OperationKind::STRUCT_DEF_FIELD_DEFAULT:
        case OperationKind::STRUCT_DEF_FIELD_VISIBILITY:
        case OperationKind::STRUCT_DEF_FIELD_IMMUTABLE:
        case OperationKind::STRUCT_DEF_METHOD:
        case OperationKind::STRUCT_DEF_VALIDATOR:
        case OperationKind::STRUCT_DEF_IMPLEMENT:
        case OperationKind::STRUCT_DEF_EXTEND:
        case OperationKind::STRUCT_DEF_SEAL:
        case OperationKind::STRUCT_SET:
        case OperationKind::STRUCT_SET_I:
        case OperationKind::INTERFACE_NAME:
        case OperationKind::INTERFACE_METHOD:
        case OperationKind::VEC_PUSH:
        case OperationKind::VEC_SET:
        case OperationKind::MAT_SET:
        case OperationKind::STR_REPLACE:
        case OperationKind::TYPE_ASSERT:
        case OperationKind::TYPE_HINT:
        case OperationKind::WRITE_FILE:
        case OperationKind::APPEND_FILE:
        case OperationKind::DELETE_FILE:
        case OperationKind::SLEEP:
        case OperationKind::STORE:
        case OperationKind::CLEAR:
        case OperationKind::TRY:
        case OperationKind::CATCH:
        case OperationKind::FINALLY:
        case OperationKind::END_TRY:
        case OperationKind::THROW:
        case OperationKind::ERR_CLEAR:
            return false;
        default:
            return true;
    }
}

bool is_mutating_first_operand(OperationKind op) {
    switch (op) {
        case OperationKind::LIST_PUSH:
        case OperationKind::LIST_INSERT:
        case OperationKind::LIST_ERASE:
        case OperationKind::LIST_CLEAR:
        case OperationKind::LIST_SORT:
        case OperationKind::LIST_REVERSE:
        case OperationKind::LIST_SET:
        case OperationKind::MAP_SET:
        case OperationKind::MAP_DELETE:
        case OperationKind::SET_ADD:
        case OperationKind::SET_DELETE:
        case OperationKind::STRUCT_DEF_NAME:
        case OperationKind::STRUCT_DEF_FIELD:
        case OperationKind::STRUCT_DEF_FIELD_DEFAULT:
        case OperationKind::STRUCT_DEF_FIELD_VISIBILITY:
        case OperationKind::STRUCT_DEF_FIELD_IMMUTABLE:
        case OperationKind::STRUCT_DEF_METHOD:
        case OperationKind::STRUCT_DEF_VALIDATOR:
        case OperationKind::STRUCT_DEF_IMPLEMENT:
        case OperationKind::STRUCT_DEF_EXTEND:
        case OperationKind::STRUCT_DEF_SEAL:
        case OperationKind::STRUCT_SET:
        case OperationKind::STRUCT_SET_I:
        case OperationKind::INTERFACE_NAME:
        case OperationKind::INTERFACE_METHOD:
        case OperationKind::VEC_PUSH:
        case OperationKind::VEC_SET:
        case OperationKind::MAT_SET:
        case OperationKind::STR_REPLACE:
        case OperationKind::TYPE_HINT:
        case OperationKind::STORE:
        case OperationKind::CLEAR:
            return true;
        default:
            return false;
    }
}

bool is_terminal(OperationKind op) {
    return op == OperationKind::HALT ||
           op == OperationKind::RET ||
           op == OperationKind::JMP ||
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

bool is_function_reference_operand(OperationKind op, size_t operand_index) {
    switch (op) {
        case OperationKind::CALL:
            return operand_index == 0;
        case OperationKind::LIST_MAP:
        case OperationKind::LIST_FILTER:
        case OperationKind::LIST_REDUCE:
        case OperationKind::STRUCT_DEF_METHOD:
            return operand_index == 2;
        case OperationKind::STRUCT_DEF_VALIDATOR:
            return operand_index == 1;
        default:
            return false;
    }
}

size_t minimum_variadic_arity(OperationKind op) {
    switch (op) {
        case OperationKind::STRUCT_INIT:
        case OperationKind::LIST_DESTRUCTURE:
        case OperationKind::STRUCT_DESTRUCTURE:
            return 2;
        default:
            return 0;
    }
}

void add(std::vector<VerificationDiagnostic>& diagnostics,
         DiagnosticSeverity severity,
         const Function& function,
         size_t pc,
         OperationKind op,
         const std::string& message) {
    VerificationDiagnostic diagnostic;
    diagnostic.severity = severity;
    diagnostic.message = message;
    diagnostic.function_name = function.name;
    diagnostic.opcode = operation_kind_to_string(op);
    diagnostic.pc = pc;
    diagnostics.push_back(std::move(diagnostic));
}

void add_program(std::vector<VerificationDiagnostic>& diagnostics,
                 DiagnosticSeverity severity,
                 const std::string& message) {
    VerificationDiagnostic diagnostic;
    diagnostic.severity = severity;
    diagnostic.message = message;
    diagnostics.push_back(std::move(diagnostic));
}

void verify_operand_shape(std::vector<VerificationDiagnostic>& diagnostics,
                          const Program& program,
                          const Function& function,
                          const Instruction& ins,
                          size_t pc) {
    if (writes_first_operand(ins.op) && !ins.operands.empty() && ins.operands[0].kind != OperandKind::Slot) {
        add(diagnostics,
            DiagnosticSeverity::Error,
            function,
            pc,
            ins.op,
            "destination operand must be a slot");
    }

    if (is_mutating_first_operand(ins.op) && !ins.operands.empty() && ins.operands[0].kind != OperandKind::Slot) {
        add(diagnostics,
            DiagnosticSeverity::Error,
            function,
            pc,
            ins.op,
            "mutating first operand must be a slot");
    }

    for (size_t i = 0; i < ins.operands.size(); i++) {
        const Operand& operand = ins.operands[i];
        if (operand.kind == OperandKind::Constant && static_cast<size_t>(operand.value) >= program.constants.size()) {
            add(diagnostics,
                DiagnosticSeverity::Error,
                function,
                pc,
                ins.op,
                "constant index @" + std::to_string(operand.value) +
                    " is out of range for " + std::to_string(program.constants.size()) + " constant(s)");
        }
        if (operand.kind == OperandKind::Address) {
            add(diagnostics,
                DiagnosticSeverity::Warning,
                function,
                pc,
                ins.op,
                "address operands are parsed but not supported by runtime read/write operations");
        }
        if (i > 0 && !is_label_target(ins.op) && !is_function_reference_operand(ins.op, i) &&
            operand.kind == OperandKind::Label) {
            add(diagnostics,
                DiagnosticSeverity::Warning,
                function,
                pc,
                ins.op,
                "label-like operand will be read as data only if the opcode supports it");
        }
    }

    if (ins.op == OperationKind::CALL && (ins.operands.empty() || ins.operands[0].kind != OperandKind::Function)) {
        add(diagnostics, DiagnosticSeverity::Error, function, pc, ins.op, "CALL requires a function operand");
    }

    if ((ins.op == OperationKind::TRY || ins.op == OperationKind::FINALLY) &&
        (ins.operands.empty() || ins.operands[0].kind != OperandKind::Label)) {
        add(diagnostics, DiagnosticSeverity::Error, function, pc, ins.op, "handler target must be a label");
    }

    if (ins.op == OperationKind::TYPE_HINT && !ins.operands.empty() && ins.operands[0].kind != OperandKind::Slot) {
        add(diagnostics, DiagnosticSeverity::Error, function, pc, ins.op, "TYPE_HINT first operand must be a slot");
    }

    for (size_t i = 0; i < ins.operands.size(); i++) {
        if (!is_function_reference_operand(ins.op, i)) {
            continue;
        }
        const Operand& operand = ins.operands[i];
        if (!is_function_name_operand(operand)) {
            add(diagnostics,
                DiagnosticSeverity::Error,
                function,
                pc,
                ins.op,
                "function reference operand must be a function name, label name, string, slot, or constant");
            continue;
        }

        const std::string static_name = function_name_from_static_operand(operand);
        if (!static_name.empty() && !program.functions.exists(static_name)) {
            add(diagnostics,
                DiagnosticSeverity::Error,
                function,
                pc,
                ins.op,
                "referenced function does not exist: " + static_name);
        }
    }
}

void verify_labels(std::vector<VerificationDiagnostic>& diagnostics,
                   const Function& function,
                   const Instruction& ins,
                   size_t pc) {
    if (!is_label_target(ins.op) || ins.operands.empty() || ins.operands[0].kind != OperandKind::Label) {
        return;
    }

    const std::string& label = ins.operands[0].strval;
    const auto it = function.labels.find(label);
    if (it == function.labels.end()) {
        add(diagnostics,
            DiagnosticSeverity::Error,
            function,
            pc,
            ins.op,
            "referenced label does not exist in this function: " + label);
        return;
    }
    if (it->second >= function.ins.size()) {
        add(diagnostics,
            DiagnosticSeverity::Error,
            function,
            pc,
            ins.op,
            "label target pc is out of range: " + label);
    }
}

void verify_try_structure(std::vector<VerificationDiagnostic>& diagnostics, const Function& function) {
    std::vector<size_t> try_stack;
    for (size_t pc = 0; pc < function.ins.size(); pc++) {
        const Instruction& ins = function.ins[pc];
        if (ins.op == OperationKind::TRY) {
            try_stack.push_back(pc);
            continue;
        }
        if (ins.op == OperationKind::FINALLY) {
            if (try_stack.empty()) {
                add(diagnostics,
                    DiagnosticSeverity::Error,
                    function,
                    pc,
                    ins.op,
                    "FINALLY used without an active TRY");
            }
            continue;
        }
        if (ins.op == OperationKind::END_TRY) {
            if (try_stack.empty()) {
                add(diagnostics,
                    DiagnosticSeverity::Error,
                    function,
                    pc,
                    ins.op,
                    "END_TRY used without an active TRY");
            } else {
                try_stack.pop_back();
            }
        }
    }

    for (size_t pc : try_stack) {
        add(diagnostics,
            DiagnosticSeverity::Warning,
            function,
            pc,
            OperationKind::TRY,
            "TRY has no matching END_TRY in linear instruction order");
    }
}

void verify_reachability(std::vector<VerificationDiagnostic>& diagnostics, const Function& function) {
    std::unordered_set<size_t> label_targets;
    for (const auto& [_, pc] : function.labels) {
        label_targets.insert(pc);
    }

    bool unreachable = false;
    for (size_t pc = 0; pc < function.ins.size(); pc++) {
        const Instruction& ins = function.ins[pc];
        if (label_targets.contains(pc)) {
            unreachable = false;
        }
        if (unreachable) {
            add(diagnostics,
                DiagnosticSeverity::Warning,
                function,
                pc,
                ins.op,
                "instruction is unreachable in straight-line control flow");
        }
        unreachable = is_terminal(ins.op);
    }
}

void verify_function(std::vector<VerificationDiagnostic>& diagnostics,
                     const Program& program,
                     const Function& function) {
    if (function.ins.empty()) {
        add_program(diagnostics,
                    DiagnosticSeverity::Warning,
                    "function '" + function.name + "' has no instructions");
    }

    for (const auto& [label, pc] : function.labels) {
        if (pc >= function.ins.size()) {
            add_program(diagnostics,
                        DiagnosticSeverity::Error,
                        "label '" + label + "' in function '" + function.name + "' targets pc " +
                            std::to_string(pc) + " outside function bounds");
        }
    }

    for (size_t pc = 0; pc < function.ins.size(); pc++) {
        const Instruction& ins = function.ins[pc];
        const OperationDefinition* definition = find_definition(ins.op);
        if (definition == nullptr) {
            add(diagnostics,
                DiagnosticSeverity::Error,
                function,
                pc,
                ins.op,
                "operation is missing from operation metadata table");
        } else {
            if (definition->arity == kVariadicArity) {
                const size_t minimum = minimum_variadic_arity(ins.op);
                if (ins.operands.size() < minimum) {
                    add(diagnostics,
                        DiagnosticSeverity::Error,
                        function,
                        pc,
                        ins.op,
                        "operand count mismatch: expected at least " + std::to_string(minimum) +
                            ", got " + std::to_string(ins.operands.size()));
                }
            } else if (definition->arity != ins.operands.size()) {
                add(diagnostics,
                    DiagnosticSeverity::Error,
                    function,
                    pc,
                    ins.op,
                    "operand count mismatch: expected " + std::to_string(definition->arity) +
                        ", got " + std::to_string(ins.operands.size()));
            }
            if (!program.imported_categories.contains(definition->category)) {
                add(diagnostics,
                    DiagnosticSeverity::Error,
                    function,
                    pc,
                    ins.op,
                    "missing import for operation category: " + std::string(definition->category));
            }
        }

        verify_labels(diagnostics, function, ins, pc);
        verify_operand_shape(diagnostics, program, function, ins, pc);
    }

    verify_try_structure(diagnostics, function);
    verify_reachability(diagnostics, function);
}

} // namespace

std::vector<VerificationDiagnostic> verify(const Program& program) {
    std::vector<VerificationDiagnostic> diagnostics;

    if (!program.functions.exists("main")) {
        add_program(diagnostics, DiagnosticSeverity::Error, "missing '.main' entry point");
    }
    if (program.functions.size() == 0) {
        add_program(diagnostics, DiagnosticSeverity::Error, "program defines no functions");
        return diagnostics;
    }

    for (size_t i = 0; i < program.functions.size(); i++) {
        verify_function(diagnostics, program, *program.functions.at(i));
    }

    return diagnostics;
}

bool has_verification_errors(const std::vector<VerificationDiagnostic>& diagnostics) {
    return std::any_of(diagnostics.begin(), diagnostics.end(), [](const VerificationDiagnostic& diagnostic) {
        return diagnostic.severity == DiagnosticSeverity::Error;
    });
}

std::string format_verification_diagnostic(const VerificationDiagnostic& diagnostic) {
    std::string out = diagnostic.severity == DiagnosticSeverity::Error ? "[verify error]" : "[verify warning]";
    if (!diagnostic.function_name.empty()) {
        out += " function=" + diagnostic.function_name;
    }
    if (diagnostic.pc != static_cast<size_t>(-1)) {
        out += " pc=" + std::to_string(diagnostic.pc);
    }
    if (!diagnostic.opcode.empty()) {
        out += " op=" + diagnostic.opcode;
    }
    out += ": " + diagnostic.message;
    return out;
}

} // namespace aic
