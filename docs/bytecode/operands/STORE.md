# `STORE` Instruction

Stores a value into a slot.


## Syntax

```
STORE <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` value to store (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Requires `<dst>` to be a slot.
- Copies `<value>` into that slot.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `memory` operation category.


## Example

```
.main
start:
  STORE $0, #1
  HALT
```


## Errors

An error is raised if:

- `<dst>` is not a slot


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
