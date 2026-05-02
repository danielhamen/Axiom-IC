# `MOD` Instruction

Returns the integer remainder.


## Syntax

```
MOD <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left operand (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Computes `lhs % rhs`.
- Both operands must be integers.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `arithmetic` operation category.


## Example

```
.main
start:
  MOD $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- either operand is not an integer
- the divisor is zero


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
