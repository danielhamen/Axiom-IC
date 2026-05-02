# `NEQ` Instruction

Tests two values for inequality.


## Syntax

```
NEQ <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left operand (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Stores the inverse of `EQ`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `comparison` operation category.


## Example

```
.main
start:
  NEQ $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
