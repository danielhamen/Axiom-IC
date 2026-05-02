# `JMP_LT` Instruction

Jumps when one number is less than another.


## Syntax

```
JMP_LT <label>, <lhs>, <rhs>
```

- `<label>` destination label (label operand)
- `<lhs>` left comparison value (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right comparison value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Converts both operands to double and compares `lhs < rhs`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `control_flow` operation category.


## Example

```
.main
start:
  JMP_LT done, #1, #1
done:
  HALT
```


## Errors

An error is raised if:

- the destination label does not exist in the current function


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
