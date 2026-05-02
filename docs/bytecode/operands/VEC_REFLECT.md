# `VEC_REFLECT` Instruction

Reflects a vector across a normal vector.


## Syntax

```
VEC_REFLECT <dst>, <v>, <normal>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<v>` vector to reflect (immediate, slot, constant, argument, or supported operand value)
- `<normal>` normal vector (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Computes `v - 2 * dot(v,n) / dot(n,n) * n`.
- Requires equal sizes and a non-zero normal.
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
  VEC_REFLECT $0, $1, $2
  HALT
```


## Errors

An error is raised if:

- vector sizes do not meet the operation requirements
- a required non-zero vector is zero


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
