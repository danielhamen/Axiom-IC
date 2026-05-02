# `POP` Instruction

Pops the top VM stack value into a destination slot.


## Syntax

```
POP <dst>
```

- `<dst>` destination slot for the popped stack value (destination slot, usually `$n`)


## Semantics

- Removes the last stack value.
- Stores that value into `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `stack` operation category.


## Example

```
.main
start:
  POP $0
  HALT
```


## Errors

An error is raised if:

- the stack is empty


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
