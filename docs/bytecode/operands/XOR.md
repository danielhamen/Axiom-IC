# `XOR` Instruction

Performs boolean XOR or integer bitwise XOR.



## Category

- `logic`

## Syntax

```
XOR <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left operand (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Boolean operands store whether the booleans differ.
- Integer operands store bitwise `lhs ^ rhs`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `logic` operation category.


## Example

```
.main
start:
  XOR $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- operands are not both booleans or both integers


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
