# `MAP_RANGE` Instruction

Normalizes a number within an input range.


## Syntax

```
MAP_RANGE <dst>, <in_min>, <in_max>
```

- `<dst>` slot containing value and receiving result (destination slot, usually `$n`)
- `<in_min>` numeric input minimum (immediate, slot, constant, argument, or supported operand value)
- `<in_max>` numeric input maximum (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads `<value>` from `<dst>`.
- Stores `(value - in_min) / (in_max - in_min)` in `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `math` operation category.


## Example

```
.main
start:
  MAP_RANGE $0, #0, #1
  HALT
```


## Errors

An error is raised if:

- `in_min` equals `in_max`


## Notes

- `<dst>` is both an input and the output for this instruction.
