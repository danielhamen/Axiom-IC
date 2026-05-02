# `VEC_LEN` Instruction

Returns the length of a vector.


## Syntax

```
VEC_LEN <dst>, <vec>
```

- `<dst>` destination slot for the length (destination slot, usually `$n`)
- `<vec>` source vector (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Stores the number of vector elements as an integer.
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
  VEC_LEN $0, $1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
