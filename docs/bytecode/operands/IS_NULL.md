# `IS_NULL` Instruction

Tests whether a value is null.



## Category

- `type`

## Syntax

```
IS_NULL <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Stores `true` only for `ValueKind::Null`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `type` operation category.


## Example

```
.main
start:
  IS_NULL $0, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
