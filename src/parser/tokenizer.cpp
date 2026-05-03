#include "tokenizer.hpp"

#include "../core/types.hpp"
#include "../util/symbols.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

namespace aic {

std::string token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::Directive:
            return "Directive";
        case TokenType::Integer:
            return "Integer";
        case TokenType::Float:
            return "Float";
        case TokenType::String:
            return "String";
        case TokenType::Dollar:
            return "Dollar";
        case TokenType::Hash:
            return "Hash";
        case TokenType::At:
            return "At";
        case TokenType::Amp:
            return "Amp";
        case TokenType::Comma:
            return "Comma";
        case TokenType::Colon:
            return "Colon";
        case TokenType::Less:
            return "Less";
        case TokenType::Greater:
            return "Greater";
        case TokenType::Newline:
            return "Newline";
        case TokenType::EndOfFile:
            return "EndOfFile";
        case TokenType::Invalid:
            return "Invalid";
    }

    throw std::runtime_error("Unknown TokenType");
}

std::vector<Token> tokenize(const std::string& in) {
    std::vector<Token> tokens;

    size_t idx = 0;
    size_t len = in.size();
    size_t line = 1;
    size_t column = 1;

    while (idx < len) {
        char ch = in[idx];
        char nextch = idx + 1 < len ? in[idx + 1] : '\0';

        if (ch == '\n') {
            tokens.push_back(Token{TokenType::Newline, "\\n", line, column});
            idx++;
            line++;
            column = 1;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(ch))) {
            idx++;
            column++;
            continue;
        }

        if (ch == '~' && nextch == '"') {
            const size_t tok_line = line;
            const size_t tok_col = column;
            idx += 2;
            column += 2;
            size_t j = idx;
            std::string contents = "";
            while (j < len) {
                char jch = in[j];

                if (jch == '\\') {
                    if (j + 1 >= len) {
                        throw std::runtime_error("Unterminated escape sequence in string");
                    }

                    char esch = in[j + 1];
                    esch = escape_char(esch);
                    contents += esch;

                    j += 2;
                    column += 2;
                    continue;
                }

                else if (jch == '"') {
                    break;
                }

                else if (jch == '\n') {
                    throw std::runtime_error("Newline occurred prior to string termination");
                }

                contents += jch;
                j++;
                column++;

                if (j == len) {
                    throw std::runtime_error("String not terminated");
                }
            }

            tokens.push_back(Token{TokenType::String, contents, tok_line, tok_col});
            idx = j + 1;
            column++;
            continue;
        }

        if (ch == ';') {
            while (idx < len && in[idx] != '\n') {
                idx++;
                column++;
            }
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '.') {
            const size_t tok_col = column;
            size_t start = idx;
            size_t j = idx;
            while (j < len && (std::isalnum(static_cast<unsigned char>(in[j])) || in[j] == '_' || in[j] == '.')) {
                j++;
            }

            std::string lexeme = in.substr(start, j - start);
            TokenType type = TokenType::Identifier;
            if (lexeme.starts_with(".") && is_symbol(lexeme.substr(1))) {
                type = TokenType::Directive;
            }

            tokens.push_back(Token{type, lexeme, line, tok_col});
            idx = j;
            column = tok_col + (j - start);
            continue;
        }

        const bool starts_signed_number =
            (ch == '-' || ch == '+') &&
            idx + 1 < len &&
            (std::isdigit(static_cast<unsigned char>(nextch)) ||
             (nextch == '.' && idx + 2 < len && std::isdigit(static_cast<unsigned char>(in[idx + 2]))));

        if (std::isdigit(static_cast<unsigned char>(ch)) || starts_signed_number) {
            const size_t tok_col = column;
            size_t start = idx;
            size_t j = idx;
            if (starts_signed_number) {
                j++;
            }

            while (j < len && std::isdigit(static_cast<unsigned char>(in[j]))) {
                j++;
            }

            TokenType num_type = TokenType::Integer;
            if (j < len && in[j] == '.' && j + 1 < len && std::isdigit(static_cast<unsigned char>(in[j + 1]))) {
                num_type = TokenType::Float;
                j++; // consume dot
                while (j < len && std::isdigit(static_cast<unsigned char>(in[j]))) {
                    j++;
                }
            }

            tokens.push_back(Token{num_type, in.substr(start, j - start), line, tok_col});
            idx = j;
            column = tok_col + (j - start);
            continue;
        }

        if (ch == '$' || ch == '#' || ch == '@' || ch == '&' || ch == ',' || ch == ':' || ch == '<' || ch == '>') {
            TokenType type = TokenType::Invalid;
            if (ch == '$') type = TokenType::Dollar;
            else if (ch == '#') type = TokenType::Hash;
            else if (ch == '@') type = TokenType::At;
            else if (ch == '&') type = TokenType::Amp;
            else if (ch == ',') type = TokenType::Comma;
            else if (ch == ':') type = TokenType::Colon;
            else if (ch == '<') type = TokenType::Less;
            else if (ch == '>') type = TokenType::Greater;

            tokens.push_back(Token{type, std::string(1, ch), line, column});
            idx++;
            column++;
            continue;
        }

        tokens.push_back(Token{TokenType::Invalid, std::string(1, ch), line, column});
        idx++;
        column++;
    }

    tokens.push_back(Token{TokenType::EndOfFile, "", line, column});

    return tokens;
}

void print_tokens(const std::vector<Token>& tokens) {
    for (const Token& token : tokens) {
        std::cout << "Token(type=" << token_type_to_string(token.type)
                  << ", lexeme='" << token.lexeme
                  << "', line=" << token.line
                  << ", column=" << token.column
                  << ")" << std::endl;
    }
}

} // namespace aic
