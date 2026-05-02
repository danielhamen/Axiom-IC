# `MAT_GET` Instruction

Reads a matrix element by flat row-major index.


## Syntax

```
MAT_GET <dst>, <mat>, <index>
```

- `<dst>` destination slot for the element (destination slot, usually `$n`)
- `<mat>` source matrix (immediate, slot, constant, argument, or supported operand value)
- `<index>` non-negative flat row-major index (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Stores the element as a float.
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
  MAT_GET $0, $1, #0
  HALT
```


## Errors

An error is raised if:

- an index is negative or out of bounds
- operands are not matrices or dimensions do not satisfy the operation


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
