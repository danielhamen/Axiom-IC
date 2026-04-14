#include "core/operations.hpp"
#include "parser/parser.hpp"
#include "parser/tokenizer.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    std::string filename = "Main.aic";
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::string in = "";
    std::string line;
    while (std::getline(file, line)) {
        in += line + "\n";
    }

    aic::register_operations();

    std::vector<std::vector<std::string>> tokens = aic::tokenize(in);
    aic::Program vm = aic::parse(tokens);

    const auto t0 = std::chrono::steady_clock::now();
    vm.exec();

    const auto t1 = std::chrono::steady_clock::now();
    const auto dt = t1 - t0;

    std::cout << std::endl << "Program executed in " << dt << std::endl;

    return 0;
}
