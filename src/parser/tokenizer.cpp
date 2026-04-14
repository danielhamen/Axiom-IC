#include "tokenizer.hpp"

#include "../core/types.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

namespace aic {

std::vector<std::vector<std::string>> tokenize(const std::string& in) {
    std::vector<std::vector<std::string>> tokens;
    std::vector<std::string> current_line;

    size_t idx = 0;
    size_t len = in.size();

    while (idx < len) {
        char ch = in[idx];
        char nextch = idx + 1 < len ? in[idx + 1] : '\0';

        if (ch == '\n') {
            if (!current_line.empty()) {
                tokens.push_back(current_line);
                current_line.clear();
            }
            idx++;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(ch))) {
            idx++;
            continue;
        }

        if (ch == '~' && nextch == '"') {
            idx += 2;
            size_t j = idx;
            std::string contents = "";
            while (j < len) {
                char jch = in[j];

                if (jch == '\\') {
                    char esch = in[j + 1];
                    esch = escape_char(esch);
                    contents += esch;

                    j += 2;
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

                if (j == len) {
                    throw std::runtime_error("String not terminated");
                }
            }

            current_line.push_back(contents);
            idx = j + 1;
            continue;
        }

        if (ch == ';') {
            while (idx < len && in[idx] != '\n') idx++;
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '.') {
            size_t j = idx;
            while (j < len && (std::isalnum(static_cast<unsigned char>(in[j])) || in[j] == '_' || in[j] == '.')) {
                j++;
            }
            current_line.push_back(in.substr(idx, j - idx));
            idx = j;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(ch))) {
            size_t j = idx;
            while (j < len && std::isdigit(static_cast<unsigned char>(in[j]))) {
                j++;
            }
            current_line.push_back(in.substr(idx, j - idx));
            idx = j;
            continue;
        }

        if (ch == '$' || ch == '#' || ch == '@' || ch == '&' || ch == ',' || ch == ':') {
            current_line.push_back(std::string(1, ch));
            idx++;
            continue;
        }

        current_line.push_back(std::string(1, ch));
        idx++;
    }

    if (!current_line.empty()) {
        tokens.push_back(current_line);
    }

    return tokens;
}

void print_tokens(std::vector<std::vector<std::string>> tokens) {
    for (std::vector<std::string> tokenline : tokens) {
        std::cout << "[";
        for (std::string token : tokenline) {
            std::cout << "'" << token << "'"
                      << " ";
        }

        std::cout << "]" << std::endl;
    }
}

} // namespace aic
