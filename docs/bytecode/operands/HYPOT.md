# `HYPOT` Instruction

Returns the Euclidean norm of two numbers.


## Syntax

```
HYPOT <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left operand (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Computes `hypot(lhs, rhs)`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `math` operation category.
- Numeric operands accept integers or floats unless the errors section says otherwise.


## Example

```
.main
start:
  HYPOT $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
