# `VEC_LERP` Instruction

Linearly interpolates between two vectors.



## Category

- `vector`

## Syntax

```
VEC_LERP <dst>, <a>, <b>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<a>` left vector (immediate, slot, constant, argument, or supported operand value)
- `<b>` right vector (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads interpolation factor `t` from `<dst>` and clamps it to `[0, 1]`.
- Stores `a + (b - a) * t` in `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `vector` operation category.
- Vectors are backed by `std::vector<double>` and print as `vec(...)`.


## Example

```
.main
start:
  VEC_NEW $1, #3
  VEC_SET $1, #0, #1
  VEC_NEW $2, #3
  VEC_SET $2, #1, #1
  VEC_LERP $0, $1, $2
  HALT
```


## Errors

An error is raised if:

- vector sizes do not meet the operation requirements


## Notes

- `<dst>` is both an input and the output for this instruction.
