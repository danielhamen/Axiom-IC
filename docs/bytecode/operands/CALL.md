# `CALL` Instruction

Calls a bytecode function.


## Syntax

```
CALL <fn>
```

- `<fn>` function operand to call (function operand)


## Semantics

- Pops the callee argument count from the stack into the call frame.
- Saves the return function and return program counter.
- Switches execution to the callee function.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `function` operation category.


## Example

```
.main
start:
  CALL example
  HALT
```


## Errors

An error is raised if:

- the operand is not a function
- the target function does not exist
- the stack does not contain enough arguments


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
