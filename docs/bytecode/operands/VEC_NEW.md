# `VEC_NEW` Instruction

Creates a vector with a fixed initial size.



## Category

- `vector`

## Syntax

```
VEC_NEW <dst>, <size>
```

- `<dst>` destination slot for the new vector (destination slot, usually `$n`)
- `<size>` non-negative integer size (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Allocates a vector of length `<size>`.
- Initializes every element to `0.0`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `vector` operation category.
- Vectors are backed by `std::vector<double>` and print as `vec(...)`.


## Example

```
.main
start:
  VEC_NEW $0, #0
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
