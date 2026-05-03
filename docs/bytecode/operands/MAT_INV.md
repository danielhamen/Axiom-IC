# `MAT_INV` Instruction

Computes the inverse of a square matrix.



## Category

- `matrix`

## Syntax

```
MAT_INV <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Uses Gauss-Jordan elimination.
- Stores `null` when the matrix is singular.
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
  MAT_INV $0, #1
  HALT
```


## Errors

An error is raised if:

- operands are not matrices or dimensions do not satisfy the operation
- a required square matrix is not square


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
