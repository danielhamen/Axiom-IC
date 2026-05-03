# `LERP` Instruction

Linearly interpolates between two numbers.



## Category

- `math`

## Syntax

```
LERP <dst>, <a>, <b>
```

- `<dst>` slot containing t and receiving result (destination slot, usually `$n`)
- `<a>` numeric start value (immediate, slot, constant, argument, or supported operand value)
- `<b>` numeric end value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads interpolation factor `t` from `<dst>` and clamps it to `[0, 1]`.
- Stores `a + (b - a) * t` in `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `math` operation category.


## Example

```
.main
start:
  LERP $0, $1, $2
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- `<dst>` is both an input and the output for this instruction.
