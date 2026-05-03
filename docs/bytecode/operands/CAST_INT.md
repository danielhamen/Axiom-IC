# `CAST_INT` Instruction

Casts a value to an integer.



## Category

- `type`

## Syntax

```
CAST_INT <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Integers pass through.
- Floats truncate toward zero.
- Booleans map to `1` or `0`.
- Strings are parsed with `std::stoll`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `type` operation category.


## Example

```
.main
start:
  CAST_INT $0, #1
  HALT
```


## Errors

An error is raised if:

- the source type is unsupported or string parsing fails


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
