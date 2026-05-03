# `MAT_IDENTITY` Instruction

Creates an identity matrix.



## Category

- `matrix`

## Syntax

```
MAT_IDENTITY <dst>, <size>
```

- `<dst>` destination slot for the matrix (destination slot, usually `$n`)
- `<size>` non-negative matrix size (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Allocates an `n x n` matrix.
- Sets diagonal elements to `1.0` and all others to `0.0`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `matrix` operation category.
- Matrices are stored as flat row-major `std::vector<double>` data with row and column metadata.


## Example

```
.main
start:
  MAT_IDENTITY $0, #0
  HALT
```


## Errors

An error is raised if:

- operands are not matrices or dimensions do not satisfy the operation


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
