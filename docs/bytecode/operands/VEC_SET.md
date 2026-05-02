# `VEC_SET` Instruction

Writes a vector element by index.


## Syntax

```
VEC_SET <vec>, <index>, <value>
```

- `<vec>` vector slot to mutate (slot operand, usually `$n`)
- `<index>` non-negative element index (non-negative integer, either immediate `#n` or an integer operand)
- `<value>` numeric replacement value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Converts `<value>` to a double.
- Writes it at `<index>`.
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
  VEC_SET $1, #0, #1
  HALT
```


## Errors

An error is raised if:

- an index is negative or out of bounds


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
