# `CAST_STRING` Instruction

Casts a value to a string.



## Category

- `type`

## Syntax

```
CAST_STRING <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Uses `Value::to_str()`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `type` operation category.


## Example

```
.main
start:
  CAST_STRING $0, #1
  HALT
```


## Errors

An error is raised if:

- the source type is unsupported or string parsing fails


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
