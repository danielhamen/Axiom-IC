#pragma once

#include "program.hpp"

#include <string>
#include <vector>

namespace aic {

enum class DiagnosticSeverity {
    Warning,
    Error
};

struct VerificationDiagnostic {
    DiagnosticSeverity severity = DiagnosticSeverity::Error;
    std::string message;
    std::string function_name;
    std::string opcode;
    size_t pc = static_cast<size_t>(-1);
};

std::vector<VerificationDiagnostic> verify(const Program& program);
bool has_verification_errors(const std::vector<VerificationDiagnostic>& diagnostics);
std::string format_verification_diagnostic(const VerificationDiagnostic& diagnostic);

} // namespace aic
