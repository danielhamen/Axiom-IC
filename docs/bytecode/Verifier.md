# Bytecode Verifier

The verifier runs after parsing and before execution. It reports structural bytecode problems early, before the VM mutates runtime state.

Verifier diagnostics have two severities:

- `verify error`: execution stops
- `verify warning`: execution continues


## Current Checks

The verifier checks:

- `.main` exists
- every function and label target is in range
- opcode metadata exists
- operand count matches the operation table
- variadic opcodes have their minimum required operands
- required operation categories were imported
- label targets exist in the current function
- static function references exist
- destination and mutating operands are writable slots
- `CALL` uses a function operand
- `TRY` and `FINALLY` target labels
- `FINALLY` and `END_TRY` have an active `TRY`
- likely unreachable straight-line instructions
- parsed address operands, which are not supported by runtime read/write paths


## Example

```aic
.import <control_flow>
.import <memory>

.main
  STORE #0, #1
  JMP missing
  HALT
```

Produces:

```text
[verify error] function=main pc=0 op=STORE: mutating first operand must be a slot
[verify error] function=main pc=1 op=JMP: referenced label does not exist in this function: missing
[verify warning] function=main pc=2 op=HALT: instruction is unreachable in straight-line control flow
```

