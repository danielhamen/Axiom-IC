# `MAT_SOLVE` Instruction

Solves a square linear system.



## Category

- `matrix`

## Syntax

```
MAT_SOLVE <dst>, <matrix>, <vector>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<matrix>` square coefficient matrix (immediate, slot, constant, argument, or supported operand value)
- `<vector>` right-hand vector (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Solves `A * x = b` using Gaussian elimination.
- Stores the solution as a vector.
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
  VEC_NEW $2, #2
  VEC_SET $2, #0, #1
  MAT_SOLVE $0, $1, $2
  HALT
```


## Errors

An error is raised if:

- operands are not matrices or dimensions do not satisfy the operation
- a required square matrix is not square
- the right-hand vector size does not equal the matrix row count
- the matrix is singular


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
