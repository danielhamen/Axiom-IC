# `JMP_IF_ZERO` Instruction

Jumps when a numeric value is zero.


## Syntax

```
JMP_IF_ZERO <label>, <condition>
```

- `<label>` destination label (label operand)
- `<condition>` condition value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Converts `<value>` to a double.
- Jumps when the value equals `0.0`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `control_flow` operation category.


## Example

```
.main
start:
  JMP_IF_ZERO done, #1
done:
  HALT
```


## Errors

An error is raised if:

- the destination label does not exist in the current function


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
