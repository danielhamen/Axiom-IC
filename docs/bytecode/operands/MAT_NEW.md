# `MAT_NEW` Instruction

Creates a matrix.


## Syntax

```
MAT_NEW <dst>, <rows>, <cols>
```

- `<dst>` destination slot for the new matrix (destination slot, usually `$n`)
- `<rows>` non-negative row count (non-negative integer, either immediate `#n` or an integer operand)
- `<cols>` non-negative column count (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Allocates a row-major matrix with `<rows> * <cols>` elements.
- Initializes every element to `0.0`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `matrix` operation category.
- Matrices are stored as flat row-major `std::vector<double>` data with row and column metadata.


## Example

```
.main
start:
  MAT_NEW $0, #0, #0
  HALT
```


## Errors

An error is raised if:

- operands are not matrices or dimensions do not satisfy the operation


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
