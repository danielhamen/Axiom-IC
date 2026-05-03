# `CLAMP` Instruction

Clamps a numeric slot between a lower and upper bound.



## Category

- `math`

## Syntax

```
CLAMP <dst>, <min>, <max>
```

- `<dst>` slot containing value and receiving result (destination slot, usually `$n`)
- `<min>` numeric lower bound (immediate, slot, constant, argument, or supported operand value)
- `<max>` numeric upper bound (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads the current value from `<dst>`.
- Stores `clamp(value, min, max)` back into `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `math` operation category.


## Example

```
.main
start:
  CLAMP $0, #0, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- `<dst>` is both an input and the output for this instruction.
