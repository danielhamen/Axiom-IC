# `JMP_IF_NOT` Instruction

Jumps when a value is not truthy.


## Syntax

```
JMP_IF_NOT <label>, <condition>
```

- `<label>` destination label (label operand)
- `<condition>` condition value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- If `<condition>` is falsey, jumps to `<label>`.
- Otherwise advances to the next instruction.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `control_flow` operation category.


## Example

```
.main
start:
  JMP_IF_NOT done, #1
done:
  HALT
```


## Errors

An error is raised if:

- the destination label does not exist in the current function


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
