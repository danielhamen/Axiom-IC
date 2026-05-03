# `VEC_PUSH` Instruction

Appends a numeric value to a vector.



## Category

- `vector`

## Syntax

```
VEC_PUSH <vec>, <value>
```

- `<vec>` vector slot to mutate (slot operand, usually `$n`)
- `<value>` numeric value to append (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Converts `<value>` to a double.
- Appends it to `<vec>`.
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
  VEC_PUSH $1, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
