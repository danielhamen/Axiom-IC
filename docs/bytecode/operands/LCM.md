# `LCM` Instruction

Computes the least common multiple of two integers.


## Syntax

```
LCM <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left operand (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Uses `std::lcm`.
- Stores the result as an integer.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `math` operation category.


## Example

```
.main
start:
  LCM $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- either operand is not an integer


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
