# `VEC_GET` Instruction

Reads a vector element by index.



## Category

- `vector`

## Syntax

```
VEC_GET <dst>, <vec>, <index>
```

- `<dst>` destination slot for the element (destination slot, usually `$n`)
- `<vec>` source vector (immediate, slot, constant, argument, or supported operand value)
- `<index>` non-negative element index (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Stores the element as a float.
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
  VEC_GET $0, $1, #0
  HALT
```


## Errors

An error is raised if:

- an index is negative or out of bounds


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
