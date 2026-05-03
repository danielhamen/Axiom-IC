# `GCD` Instruction

Computes the greatest common divisor of two integers.



## Category

- `math`

## Syntax

```
GCD <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left operand (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Uses `std::gcd`.
- Stores the result as an integer.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `math` operation category.


## Example

```
.main
start:
  GCD $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- either operand is not an integer


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
