# `JMP` Instruction

Jumps unconditionally to a label in the current function.


## Syntax

```
JMP <label>
```

- `<label>` destination label in the current function (label operand)


## Semantics

- Sets the program counter to the destination label.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `control_flow` operation category.


## Example

```
.main
start:
  JMP done
done:
  HALT
```


## Errors

An error is raised if:

- the destination label does not exist in the current function


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
