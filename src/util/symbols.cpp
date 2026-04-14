#include "symbols.hpp"

#include <cctype>

namespace aic {

bool is_symbol(const std::string& str) {
    if (str.empty()) return false;

    unsigned char first = static_cast<unsigned char>(str[0]);
    if (!std::isalpha(first) && str[0] != '_') return false;

    for (unsigned char ch : str) {
        if (!std::isalnum(ch) && ch != '_') return false;
    }

    return true;
}

bool is_str_tok_directive(std::string tok) {
    return tok.starts_with(".") && is_symbol(tok.substr(1));
}

} // namespace aic
