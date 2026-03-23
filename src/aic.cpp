#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <string>
#include <cstdint>
using namespace std;

enum class Operation : int16_t {
    ADD, HALT
};

enum class OperandKind : uint8_t {
    None,
    Slot,
    Immediate,
    Constant,
    Label,
    Address
};

struct Operand {
    OperandKind kind;
    int64_t value;

    bool is_none() const {
        return kind == OperandKind::None;
    }

};

struct Instruction {
    Operation op;
    Operand x;
    Operand y;
    Operand z;
};

enum class ValueKind : uint8_t {
    Integer,
    String,
    Boolean,
    Null
};

struct Value {
    ValueKind kind;
    string s;
    int i;
    bool b;

    bool is_str() const { return kind == ValueKind::String; }
    bool is_int() const { return kind == ValueKind::Integer; }
    bool is_bool() const { return kind == ValueKind::Boolean; }
    bool is_null() const { return kind == ValueKind::Null; }
};

struct Function {
    vector<Instruction> ins;
    string name;
    unordered_map<string, size_t> labels;
};

struct Program {
    size_t pc; // program counter
    size_t fc; // function counter (current function)
    bool halted = false;
    vector<Function> functions;
    vector<Value> constants;

    /**
     * Executes a given instruction
     *
     * Most instructions increment the program counter
     * and it is not necessary to increment the pc after resolve
     */
    void resolve(Instruction ins) {
        Operation op = ins.op;
        Operand x = ins.x;
        Operand y = ins.y;
        Operand z = ins.z;

        switch (op) {
        case Operation::ADD:
            pc++;
            return;
        case Operation::HALT:
            halted = true;
            return;
        default:
            throw runtime_error("Unmatched operation found in resolve()");
        }
    };

    /**
     * Executes the program
     */
    void exec() {
        while (!halted) {
            const Instruction& ins = functions.at(fc).ins.at(pc);
            resolve(ins);
        }
    }
};

vector<vector<string>> tokenize(const string& in) {
    vector<vector<string>> tokens;
    vector<string> current_line;

    size_t idx = 0;
    size_t len = in.size();

    while (idx < len) {
        char ch = in[idx];

        // newline → push current line
        if (ch == '\n') {
            if (!current_line.empty()) {
                tokens.push_back(current_line);
                current_line.clear();
            }
            idx++;
            continue;
        }

        // whitespace
        if (isspace(ch)) {
            idx++;
            continue;
        }

        // comment (; until end of line)
        if (ch == ';') {
            while (idx < len && in[idx] != '\n') idx++;
            continue;
        }

        // identifiers / keywords (ADD, INT, start, true, etc.)
        if (isalpha(ch) || ch == '.') {
            size_t j = idx;
            while (j < len && (isalnum(in[j]) || in[j] == '_' || in[j] == '.')) {
                j++;
            }
            current_line.push_back(in.substr(idx, j - idx));
            idx = j;
            continue;
        }

        // numbers
        if (isdigit(ch)) {
            size_t j = idx;
            while (j < len && isdigit(in[j])) {
                j++;
            }
            current_line.push_back(in.substr(idx, j - idx));
            idx = j;
            continue;
        }

        // single-character tokens ($, #, ,, :)
        if (ch == '$' || ch == '#' || ch == ',' || ch == ':') {
            current_line.push_back(string(1, ch));
            idx++;
            continue;
        }

        // fallback (unknown char → still tokenize it)
        current_line.push_back(string(1, ch));
        idx++;
    }

    // push last line if needed
    if (!current_line.empty()) {
        tokens.push_back(current_line);
    }

    return tokens;
}

/**
 * checks whether `str` is a valid symbol
 */
 bool is_symbol(const string& str) {
     if (str.empty()) return false;

     unsigned char first = static_cast<unsigned char>(str[0]);
     if (!isalpha(first) && str[0] != '_') return false;

     for (unsigned char ch : str) {
         if (!isalnum(ch) && ch != '_') return false;
     }

     return true;
 }

void print_tokens(vector<vector<string>> tokens) {
    for (vector<string> tokenline : tokens) {
        cout << "[";
        for (string token : tokenline) {
            cout << "'" << token << "'" << " ";
        }

        cout << "]" << endl;
    }
}

Program parse(vector<vector<string>> tokens) {
    Program vm;
    vm.pc = 0;

    size_t i = 0;
    while (i < tokens.size()) {
        vector<string> line = tokens[i];

        // line empty
        if (line.empty()) {
            i++;
            continue;
        }

        string head = line.at(0);

        // directive
        if (head.starts_with(".")) {
            string directive = head.substr(1);

            // constant pool
            if (directive == "const") {
                // .const statements should be standalone
                if (line.size() != 1)
                    throw runtime_error("Too many elements to unpack in `.const` directive; unary directives must not take any additional tokens");
            }

            // include (future)
            if (directive == "include") {
                throw runtime_error("`include` directives are not yet implemented");
            }
        }

        // label
        else if (line.size() >= 2 && is_symbol(head) && line.at(1) == ":") {
            // ensure standalone label
            if (line.size() > 2)
                throw runtime_error("Too many elements to unpack in label.");
        }

        i++;
    }


    return vm;
}

int main() {
    string filename = "Main.aic";
    ifstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("Could not open file");
    }

    string in = "";
    string line;
    while (getline(file, line)) {
        in += line + "\n";
    }

    vector<vector<string>> tokens = tokenize(in);
    print_tokens(tokens);
    Program vm = parse(tokens);

    return 0;
}
