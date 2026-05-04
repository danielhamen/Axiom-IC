# `LOAD` Instruction

Copies a value into a destination operand.



## Category

- `memory`

## Syntax

```
LOAD <dst>, <src>
```

- `<dst>` destination operand (destination slot, usually `$n`)
- `<src>` source operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads `<src>`.
- Writes that value to `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `memory` operation category.


## Example

```
.main
start:
  LOAD $0, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- The public and internal operation name is `LOAD`.
- `$n` operands read and write slots in the active function call frame.
