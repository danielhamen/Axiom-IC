#pragma once

#include <string>
#include <vector>

namespace aic {

std::vector<std::vector<std::string>> tokenize(const std::string& in);
void print_tokens(std::vector<std::vector<std::string>> tokens);

} // namespace aic
