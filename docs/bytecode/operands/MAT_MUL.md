# `MAT_MUL` Instruction

Multiplies two matrices.


## Syntax

```
MAT_MUL <dst>, <a>, <b>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<a>` left matrix (immediate, slot, constant, argument, or supported operand value)
- `<b>` right matrix or vector (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Requires `lhs.cols == rhs.rows`.
- Stores a new matrix with shape `lhs.rows x rhs.cols`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `matrix` operation category.
- Matrices are stored as flat row-major `std::vector<double>` data with row and column metadata.


## Example

```
.main
start:
  MAT_NEW $1, #2, #2
  MAT_SET $1, #0, #1
  MAT_SET $1, #3, #1
  MAT_NEW $2, #2, #2
  MAT_MUL $0, $1, $2
  HALT
```


## Errors

An error is raised if:

- operands are not matrices or dimensions do not satisfy the operation


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
