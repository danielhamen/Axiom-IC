# `VEC_NORM` Instruction

Normalizes a vector.



## Category

- `vector`

## Syntax

```
VEC_NORM <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Divides each element by vector magnitude.
- Stores a new unit vector.
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
  VEC_NORM $0, #1
  HALT
```


## Errors

An error is raised if:

- vector sizes do not meet the operation requirements
- a required non-zero vector is zero


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
