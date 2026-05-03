# `CAST_FLOAT` Instruction

Casts a value to a float.



## Category

- `type`

## Syntax

```
CAST_FLOAT <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Integers and booleans are converted numerically.
- Floats pass through.
- Strings are parsed with `std::stod`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `type` operation category.


## Example

```
.main
start:
  CAST_FLOAT $0, #1
  HALT
```


## Errors

An error is raised if:

- the source type is unsupported or string parsing fails


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
