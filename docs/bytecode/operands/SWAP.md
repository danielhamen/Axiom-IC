# `SWAP` Instruction

Swaps two operands.


## Syntax

```
SWAP <a>, <b>
```

- `<a>` first operand (immediate, slot, constant, argument, or supported operand value)
- `<b>` second operand (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads both operands.
- Writes the first value to the second operand and the second value to the first operand.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `memory` operation category.


## Example

```
.main
start:
  SWAP $1, $2
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
