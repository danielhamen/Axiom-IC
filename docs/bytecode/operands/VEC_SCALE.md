# `VEC_SCALE` Instruction

Multiplies a vector by a scalar.


## Syntax

```
VEC_SCALE <dst>, <vec>, <scale>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<vec>` source vector (immediate, slot, constant, argument, or supported operand value)
- `<scale>` numeric scale factor (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Converts `<scale>` to a double.
- Stores a new vector.
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
  VEC_SCALE $0, $1, #1
  HALT
```


## Errors

An error is raised if:

- vector sizes do not meet the operation requirements


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
