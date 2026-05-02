# `CALL` Instruction

Calls a bytecode function.


## Syntax

```
CALL <fn>
```

- `<fn>` function operand to call (function operand)


## Semantics

- Pops the callee argument count from the stack into the call frame.
- If pending `ARG` or `KWARG` values exist, consumes those instead of legacy stack arguments.
- Saves the return function and return program counter.
- Creates a fresh local slot frame for the callee.
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

- `$n` slots inside the callee are local to that call and do not overwrite caller slots.
- `ARG` and `KWARG` provide the preferred explicit call-argument ABI.
- Legacy `PUSH` plus `CALL` positional arguments still work when no pending `ARG` or `KWARG` values exist.
