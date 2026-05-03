# `CLEAR` Instruction

Sets an operand to null.



## Category

- `memory`

## Syntax

```
CLEAR <target>
```

- `<target>` operand to set to null (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Writes `null` to `<target>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `memory` operation category.


## Example

```
.main
start:
  CLEAR $1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
