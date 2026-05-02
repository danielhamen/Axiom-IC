# `MAT_ROWS` Instruction

Returns the row count of a matrix.


## Syntax

```
MAT_ROWS <dst>, <mat>
```

- `<dst>` destination slot for row count (destination slot, usually `$n`)
- `<mat>` source matrix (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Stores the number of rows as an integer.
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
  MAT_ROWS $0, $1
  HALT
```


## Errors

An error is raised if:

- operands are not matrices or dimensions do not satisfy the operation


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
