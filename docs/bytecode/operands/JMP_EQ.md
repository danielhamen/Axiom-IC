# `JMP_EQ` Instruction

Jumps when two values are equal.


## Syntax

```
JMP_EQ <label>, <lhs>, <rhs>
```

- `<label>` destination label (label operand)
- `<lhs>` left comparison value (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right comparison value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Uses VM equality for comparison.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `control_flow` operation category.


## Example

```
.main
start:
  JMP_EQ done, #1, #1
done:
  HALT
```


## Errors

An error is raised if:

- the destination label does not exist in the current function


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
