#include "operations.hpp"

namespace aic {

std::unordered_map<std::string, Operation> operation_by_name;

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

    Operation OP_PUSH{
        OperationKind::PUSH,
        "PUSH",
        1
    };

    Operation OP_POP{
        OperationKind::POP,
        "POP",
        1
    };

    Operation OP_CALL{
        OperationKind::CALL,
        "CALL",
        1
    };

    Operation OP_RET{
        OperationKind::RET,
        "RET",
        0
    };

    Operation OP_RETVAL{
        OperationKind::RETVAL,
        "RETVAL",
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
    operation_by_name["PUSH"] = OP_PUSH;
    operation_by_name["POP"] = OP_POP;
    operation_by_name["CALL"] = OP_CALL;
    operation_by_name["RET"] = OP_RET;
    operation_by_name["RETVAL"] = OP_RETVAL;
}

} // namespace aic
