#include <chrono>
#include <ctime>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <unordered_map>
#include <format>
#include <vector>
#include <fstream>
#include <string>
#include <cstdint>
using namespace std;
using namespace chrono;

enum class Directive : int8_t {
    Constant,
    Function,
    None,
    Include
};

enum class OperationKind : int16_t {
    // Arithmetic
    ADD, SUB, MUL, DIV, MOD,

    // Control flow
    HALT, JMP,

    // I/O
    PRINT
};

struct Operation {
    OperationKind kind;
    string name;
    size_t arity;
};

unordered_map<string, Operation> operation_by_name;

void register_operations() {
    Operation OP_ADD{
        OperationKind::ADD,
        "ADD",
        3
    };

    Operation OP_SUB{
        OperationKind::SUB,
        "SUB",
        3
    };

    Operation OP_MUL{
        OperationKind::MUL,
        "MUL",
        3
    };

    Operation OP_DIV{
        OperationKind::DIV,
        "DIV",
        3
    };


    Operation OP_MOD{
        OperationKind::MOD,
        "MOD",
        3
    };

    Operation OP_JMP{
        OperationKind::JMP,
        "JMP",
        1
    };

    Operation OP_HALT{
        OperationKind::HALT,
        "HALT",
        0
    };

    Operation OP_PRINT{
        OperationKind::PRINT,
        "PRINT",
        1
    };

    operation_by_name["ADD"] = OP_ADD;
    operation_by_name["SUB"] = OP_SUB;
    operation_by_name["MUL"] = OP_MUL;
    operation_by_name["DIV"] = OP_DIV;
    operation_by_name["MOD"] = OP_MOD;

    operation_by_name["HALT"] = OP_HALT;
    operation_by_name["JMP"] = OP_JMP;

    operation_by_name["PRINT"] = OP_PRINT;
}

char escape_char(char in) {
    switch (in) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '0':
            return '\0';
        default:
            return in;
    }
}

enum class OperandKind : uint8_t {
    None,      // ""
    Slot,      // "$<n>"
    Immediate, // "#<n>"
    Constant,  // "@<n>"
    Label,     // plain label name
    Address    // "<&n>" where to jump in the instruction array
};

unordered_map<string, OperandKind> operand_kind_map = {
    {"None", OperandKind::None},
    {"Slot", OperandKind::Slot},
    {"Immediate", OperandKind::Immediate},
    {"Constant", OperandKind::Constant},
    {"Label", OperandKind::Label},
    {"Address", OperandKind::Address},
};

struct Operand {
    OperandKind kind;
    int64_t value;
    string strval;

    bool is_none() const {
        return kind == OperandKind::None;
    }

    string to_str() const {
        return format("Operand(kind={0}, value={1}, strval=2)", kindstr(), value, strval);
    }

    string kindstr() const {
        for (auto entry : operand_kind_map) {
            if (entry.second == kind)
                return entry.first;
        }

        throw runtime_error("Unmatched OperandKind");
    }

};

struct Instruction {
    OperationKind op;
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
    string s{};
    int64_t i = 0;
    bool b = false;

    bool is_str() const { return kind == ValueKind::String; }
    bool is_int() const { return kind == ValueKind::Integer; }
    bool is_bool() const { return kind == ValueKind::Boolean; }
    bool is_null() const { return kind == ValueKind::Null; }
    string to_str() const {
        switch (kind) {
            case ValueKind::String:
                return s;
            case ValueKind::Boolean:
                return b ? "true" : "false";
            case ValueKind::Null:
                return "null";
            case ValueKind::Integer:
                return to_string(i);
        }

        throw runtime_error("Unmatched ValueKind");
    }
};

/**
 * Represents an in-code function
 */
struct Function {
    vector<Instruction> ins;
    string name;
    unordered_map<string, size_t> labels;
};

/**
 * Represents a function list with operations to lookup
 */
struct FunctionList {
private:
    vector<Function> functions;

public:
    bool exists(string fn_name) {
        for (auto& fn : functions) {
            if (fn_name == fn.name) {
                return true;
            }
        }

        return false;
    }

    Function* find(string fn_name) {
        for (auto& fn : functions) {
            if (fn_name == fn.name) {
                return &fn;
            }
        }

        return nullptr;
    }

    Function* at(size_t idx) {
        if (idx >= functions.size()) {
            throw runtime_error("Error referencing function at index " + to_string(idx) + " as it exceeds the size of the function map");
        }

        return &functions.at(idx);
    }

    size_t index_of(string fn_name) {
        for (int i = 0; i < functions.size(); i++) {
            auto& fn = functions.at(i);
            if (fn_name == fn.name) {
                return i;
            }
        }

        throw runtime_error("Error indexing function that does not exist");
    }

    void insert(Function& fn) {
        functions.push_back(fn);
    }

    const size_t size() {
        return functions.size();
    }
};

struct Program {
    size_t pc; // program counter
    size_t fc; // function counter (current function)
    bool halted = false;
    FunctionList functions;
    vector<Value> constants;
    vector<Value> memory;

    Value& slot(size_t index) {
        if (index >= memory.size()) {
            memory.resize(index + 1);
        }
        return memory[index];
    }


    /**
     * Executes a given instruction
     *
     * Most instructions increment the program counter
     * and it is not necessary to increment the pc after resolve
     */
    void resolve(const Instruction& ins) {
        const OperationKind& op = ins.op;
        const Operand& x = ins.x;
        const Operand& y = ins.y;
        const Operand& z = ins.z;

        switch (op) {
        case OperationKind::ADD:{
            // get lhs and rhs values while enforcing types
            Value lhs = read_operand_strict(y, ValueKind::Integer);
            Value rhs = read_operand_strict(z, ValueKind::Integer);

            Value out{};
            out.kind = ValueKind::Integer;
            out.i = lhs.i + rhs.i;

            write_operand(x, out);
            pc++;
            return;
        }
        case OperationKind::MUL: {
            // get lhs and rhs values while enforcing types
            Value lhs = read_operand_strict(y, ValueKind::Integer);
            Value rhs = read_operand_strict(z, ValueKind::Integer);

            Value out{};
            out.kind = ValueKind::Integer;
            out.i = lhs.i * rhs.i;

            write_operand(x, out);
            pc++;
            return;
        }
        case OperationKind::SUB:{
            // get lhs and rhs values while enforcing types
            Value lhs = read_operand_strict(y, ValueKind::Integer);
            Value rhs = read_operand_strict(z, ValueKind::Integer);

            Value out{};
            out.kind = ValueKind::Integer;
            out.i = lhs.i - rhs.i;

            write_operand(x, out);
            pc++;
            return;
        }
        case OperationKind::DIV:{
            // get lhs and rhs values while enforcing types
            Value lhs = read_operand_strict(y, ValueKind::Integer);
            Value rhs = read_operand_strict(z, ValueKind::Integer);

            Value out{};
            out.kind = ValueKind::Integer;
            out.i = lhs.i / rhs.i;

            write_operand(x, out);
            pc++;
            return;
        }
        case OperationKind::MOD:{
            // get lhs and rhs values while enforcing types
            Value lhs = read_operand_strict(y, ValueKind::Integer);
            Value rhs = read_operand_strict(z, ValueKind::Integer);

            Value out{};
            out.kind = ValueKind::Integer;
            out.i = lhs.i % rhs.i;

            write_operand(x, out);
            pc++;
            return;
        }
        case OperationKind::HALT:{
            halted = true;
            return;
        }
        case OperationKind::JMP:{
            // ensure address
            if (x.kind != OperandKind::Label) {
                throw runtime_error("Expected Label after JMP");
            }

            const auto& labels = functions.at(fc)->labels;
            const string& label_name = x.strval;

            // ensure label exists
            if (!labels.contains(label_name))
                throw runtime_error("Reference to label that does not exist in the current scope");

            size_t dst_pc = labels.at(label_name);
            pc = dst_pc;

            return;
        }
        case OperationKind::PRINT:{
            if (x.is_none() || !y.is_none() || !z.is_none())
                throw runtime_error("PRINT must have exactly one parameter.");

            Value v = read_operand(x);
            cout << v.to_str();

            pc++;
            return;
        }
        default:
            throw runtime_error("Unmatched operation found in resolve()");
        }
    };

    /**
     * Executes the program
     */
    void exec() {
        if (!functions.exists("main")) {
            throw runtime_error("Error no '.main' entry point");
        }

        fc = functions.index_of("main");

        while (!halted) {
            const Instruction& ins = functions.at(fc)->ins.at(pc);
            resolve(ins);
        }
    }

    /**
     * returns the actual Value of an operand and decodes
     * it by its type ('@' for const, '$' for slot, etc)
     */
    Value read_operand(const Operand& op) {
        Value v{};

        switch (op.kind) {
            case OperandKind::None:
                v.kind = ValueKind::Null;
                break;
            case OperandKind::Slot:
                v = slot(op.value);
                break;
            case OperandKind::Immediate:
                v.kind = ValueKind::Integer;
                v.i = op.value;
                break;
            case OperandKind::Constant:
                // check constant index is within bounds of constant vector
                if (op.value >= constants.size()) {
                    throw runtime_error("Cannot index constant map at index " +
                        to_string(op.value) +
                        " as it exceeds the bounds of the constant vector.");
                }

                v = constants.at(op.value);
                break;
            default:
                throw runtime_error("Operand cannot be resolved in read_operand");
        }

        return v;
    }

    /**
     * same as `read_operand` but if the value is not `enforced_type`,
     * error will occcur
     */
    Value read_operand_strict(const Operand& op, ValueKind enforced_type) {
        Value v = read_operand(op);
        if (v.kind != enforced_type) {
            throw runtime_error("Unexepected type.");
        }

        return v;
    }

    void write_operand(const Operand& op, const Value& v) {
        if (op.kind != OperandKind::Slot) {
            throw runtime_error("write_operand requires a slot destination");
        }

        slot(op.value) = v;
    }

};

vector<vector<string>> tokenize(const string& in) {
    vector<vector<string>> tokens;
    vector<string> current_line;

    size_t idx = 0;
    size_t len = in.size();

    while (idx < len) {
        char ch = in[idx];
        char nextch = idx+1 < len ? in[idx+1] : '\0';

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

        // string parsing
        if (ch == '~' && nextch == '"') {
            // parse string until ending quote
            // also cause an error if string not terminated
            idx+=2; // increment by two to account for '~"'
            size_t j = idx;
            string contents = ""; // final string
            while (j < len) {
                char jch = in[j]; //j-th character

                // escape sequence
                if (jch == '\\') {
                    char esch = in[j+1]; //escapted character
                    esch = escape_char(esch);
                    contents += esch;

                    j+=2; // skip next char (the char we just escaped)
                    continue;
                }

                else if (jch == '"') {
                    break;
                }

                else if (jch == '\n') {
                    throw runtime_error("Newline occurred prior to string termination");
                }

                contents += jch;
                j++;

                // string is not terminated at reached eof
                if (j == len ) {
                    throw runtime_error("String not terminated");
                }
            }

            // finally append the string as a single token
            current_line.push_back(contents);
            idx = j + 1;
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
        if (ch == '$' || ch == '#' || ch == '@' || ch == '&' || ch == ',' || ch == ':') {
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

bool is_str_tok_directive(string tok) {
    return tok.starts_with(".") && is_symbol(tok.substr(1));
}

Program parse(vector<vector<string>> tokens) {
    Program vm;
    vm.pc = 0;
    Directive current_directive = Directive::None;

    size_t idx = 0;
    while (idx < tokens.size()) {
        const vector<string>& line = tokens[idx];

        // line empty
        if (line.empty()) {
            idx++;
            continue;
        }

        string head = line.at(0);

        // directive
        if (is_str_tok_directive(head)) {
            string directive = head.substr(1);

            // constant pool
            if (directive == "const") {
                // .const statements should be standalone
                if (line.size() != 1)
                    throw runtime_error("Too many elements to unpack in `.const` directive; unary directives must not take any additional tokens");

                // update current directive
                current_directive = Directive::Constant;

                idx++;
                continue;
            }

            // include (future)
            else if (directive == "include") {
                // update current directive
                current_directive = Directive::Include;

                throw runtime_error("`include` directives are not yet implemented");
                idx++;
                continue;
            }

            // function
            else {
                // update current directive
                current_directive = Directive::Function;

                // set the current function counter to the last index
                // of the program function vector so when the next instruction
                // is resolved it can be assigned to this function
                vm.fc = vm.functions.size();

                // template fn metadata
                string& fn_name = directive;
                vector<Instruction> fn_ins;
                unordered_map<string, size_t> fn_labels;

                // initialize function with blank parameters
                Function fn{
                    fn_ins,
                    fn_name,
                    fn_labels
                };
                vm.functions.insert(fn);

                idx++;
                continue;
            }
        }

        // label
        else if (line.size() >= 2 && is_symbol(head) && line.at(1) == ":") {
            // ensure standalone label
            if (line.size() > 2)
                throw runtime_error("Too many elements to unpack in label.");

            // extract label
            string label_name = head;

            // set label to be the next instruction after label in current function
            Function* curr_fn = vm.functions.at(vm.fc);
            curr_fn->labels[label_name] = curr_fn->ins.size();

            idx++;
            continue;
        }

        // =====
        // now only instructional tokens are present and no markings
        // =====

        // parse constant declaration
        if (current_directive == Directive::Constant) {
            if (line.empty())
                throw runtime_error("Empty line found in constant definition scope");

            string type = line[0];
            if (type == "INT") {
                int param = stoi(line[1]);
                Value v;
                v.kind = ValueKind::Integer;
                v.i = param;
                vm.constants.push_back(v);
            } else if (type == "BOOL") {
                if (line.size() != 2 || (line[1] != "true" && line[1] != "false")) {
                    throw runtime_error("BOOL requires true or false");
                }

                bool param = line[1] == "true";
                Value v;
                v.kind = ValueKind::Boolean;
                v.b = param;
                vm.constants.push_back(v);
            } else if (type == "NULL") {
                Value v;
                v.kind = ValueKind::Null;
                vm.constants.push_back(v);
            } else if (type == "STR") {
                string param = line[1];
                Value v;
                v.kind = ValueKind::String;
                v.s = param;
                vm.constants.push_back(v);
            } else {
                throw runtime_error("Unknown type in constant list '" + type + "'");
            }

            idx++;
            continue;
        }

        // should exist a physical instruction now
        string ins_opname = head;
        auto it = operation_by_name.find(ins_opname);
        if (it == operation_by_name.end()) {
            throw runtime_error("Unknown instruction: " + ins_opname);
        }
        const Operation& ins_op = it->second;

        Instruction ins;
        ins.op = ins_op.kind;

        // get parameter values
        size_t j = 1; // start at 1 to skip opname
        size_t param_idx = 0;
        Operand null_op{OperandKind::None};
        ins.x = null_op;
        ins.y = null_op;
        ins.z = null_op;

        while (j < line.size()) {
            const string& tok = line[j];
            Operand op{};

            // operands that require a following token
            if (tok == "$" || tok == "#" || tok == "@" || tok == "&") {
                if (j + 1 >= line.size()) {
                    throw runtime_error("Missing value after operand prefix.");
                }

                const string& next = line[j + 1];

                if (tok == "$") {
                    op.kind = OperandKind::Slot;
                    op.value = stoi(next);
                }
                else if (tok == "#") {
                    op.kind = OperandKind::Immediate;
                    op.value = stoi(next);
                }
                else if (tok == "@") {
                    op.kind = OperandKind::Constant;
                    op.value = stoi(next);
                }
                else if (tok == "&") {
                    op.kind = OperandKind::Address;
                    op.value = stoi(next);
                }

                j += 2;
            }
            // label
            else if (is_symbol(tok)) {
                op.kind = OperandKind::Label;
                op.strval = tok;
                j += 1;
            }
            // comma separates operands
            else if (tok == ",") {
                param_idx++;
                j += 1;
                continue;
            }
            else {
                throw runtime_error("Unknown token in instruction/parameter parsing: " + tok);
            }

            if (param_idx == 0) {
                ins.x = op;
            }
            else if (param_idx == 1) {
                ins.y = op;
            }
            else if (param_idx == 2) {
                ins.z = op;
            }
            else {
                throw runtime_error("Too many operands in instruction.");
            }
        }

        // enforce operation arity
        size_t expected_arity = ins_op.arity;
        size_t recieved_arity =
            (ins.x.is_none() ? 0 : 1) +
            (ins.y.is_none() ? 0 : 1) +
            (ins.z.is_none() ? 0 : 1);
        if (expected_arity != recieved_arity) {
            throw runtime_error("Error: '" + ins_op.name + "' takes exactly " + to_string(expected_arity) + " arguments. Recieved " + to_string(recieved_arity));
        }

        // append instruction
        vm.functions.at(vm.fc)->ins.push_back(ins);

        idx++;
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

    // regisater all operations
    register_operations();

    vector<vector<string>> tokens = tokenize(in);
    Program vm = parse(tokens);

    const auto t0 = steady_clock::now();
    vm.exec();

    const auto t1 = steady_clock::now();
    const auto dt = t1 - t0;

    cout << endl << "Program executed in " << dt << endl;

    return 0;
}
