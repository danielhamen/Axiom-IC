# `OR` Instruction

Performs boolean OR or integer bitwise OR.



## Category

- `logic`

## Syntax

```
OR <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left operand (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Boolean operands store logical `lhs || rhs`.
- Integer operands store bitwise `lhs | rhs`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `logic` operation category.


## Example

```
.main
start:
  OR $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- operands are not both booleans or both integers


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
