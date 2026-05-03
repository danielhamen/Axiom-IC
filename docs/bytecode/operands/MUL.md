# `MUL` Instruction

Multiplies two numeric values.



## Category

- `arithmetic`

## Syntax

```
MUL <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left operand (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Computes `lhs * rhs`. Integer * integer stores an integer; any float operand stores a float.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `arithmetic` operation category.
- Numeric operands accept integers or floats unless the errors section says otherwise.


## Example

```
.main
start:
  MUL $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
