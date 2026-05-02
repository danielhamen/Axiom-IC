# `VEC_CROSS` Instruction

Computes the 3D cross product of two vectors.


## Syntax

```
VEC_CROSS <dst>, <a>, <b>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<a>` left vector (immediate, slot, constant, argument, or supported operand value)
- `<b>` right vector (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Requires both vectors to have exactly 3 elements.
- Stores a new 3-element vector.
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
  VEC_CROSS $0, $1, $2
  HALT
```


## Errors

An error is raised if:

- vector sizes do not meet the operation requirements


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
