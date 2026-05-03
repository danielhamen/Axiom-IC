# `PUSH` Instruction

Pushes a value onto the VM stack.



## Category

- `stack`

## Syntax

```
PUSH <value>
```

- `<value>` value to push onto the VM stack (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads the operand value.
- Appends it to the stack.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `stack` operation category.


## Example

```
.main
start:
  PUSH #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
