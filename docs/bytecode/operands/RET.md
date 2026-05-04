# `RET` Instruction

Returns from the current function.



## Category

- `function`

## Syntax

```
RET
RET <value>
```

- `<value>` optional return value pushed to the caller's stack.


## Semantics

- If no call frame exists, halts the program.
- Otherwise optionally records `<value>`, restores the caller function and program counter.
- Pushes an inline `RET <value>` or pending `RETVAL` value onto the caller stack.
- Discards the returning function's local slot frame.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `function` operation category.


## Example

```
.main
start:
  RET #4
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Returning from top-level execution halts the program.
- Returned values are transported through the stack; local slots are not returned.
- `RETVAL <value>` followed by `RET` remains valid; `RET <value>` is shorthand for simple returns.
