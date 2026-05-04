#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace aic {

enum class TokenType {
    Identifier,
    Directive,
    Integer,
    Float,
    String,
    Dollar,
    Hash,
    At,
    Amp,
    Comma,
    Colon,
    LeftBrace,
    RightBrace,
    Backslash,
    Slash,
    Less,
    Greater,
    Newline,
    EndOfFile,
    Invalid
};

struct Token {
    TokenType type;
    std::string lexeme;
    size_t line;
    size_t column;
};

std::string token_type_to_string(TokenType type);
std::vector<Token> tokenize(const std::string& in);
void print_tokens(const std::vector<Token>& tokens);

} // namespace aic
