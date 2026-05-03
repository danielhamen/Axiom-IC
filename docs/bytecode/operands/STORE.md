# `STORE` Instruction

Stores a value into a slot.



## Category

- `memory`

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

- `$n` writes update the active function call frame. Caller slots are not affected by callee slot writes.
