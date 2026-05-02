# `RETVAL` Instruction

Sets the current function return value.


## Syntax

```
RETVAL <value>
```

- `<value>` value to return from the current function (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Stores the operand in the active call frame.
- The value is pushed onto the caller stack when `RET` executes.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `function` operation category.


## Example

```
.main
start:
  RETVAL #1
  HALT
```


## Errors

An error is raised if:

- it is used outside a called function


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
