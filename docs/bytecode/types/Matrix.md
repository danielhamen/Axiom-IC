# Matrix

`Matrix` is a two-dimensional numeric container stored in flat row-major order.


## Representation

- Runtime kind: `ValueKind::Matrix`
- Storage field: `Value::matrix`
- C++ storage type: `std::vector<double>`
- Shape fields: `Value::rows`, `Value::cols`
- Type name returned by `TYPEOF`: `Matrix`


## Construction

Matrices are created with `MAT_NEW`:

```aic
MAT_NEW $0, #2, #3
```

The row and column operands are non-negative integers. Elements are initialized to `0.0`.

Identity matrices are created with `MAT_IDENTITY`:

```aic
MAT_IDENTITY $0, #3
```


## Storage Layout

Matrix elements are stored in row-major order.

For a matrix with `cols` columns, flat index is:

```text
index = row * cols + col
```

`MAT_GET` and `MAT_SET` use this flat index.


## String Form

Matrices print as `mat(rowsxcols)[...]`:

```text
mat(2x2)[[1.000000, 0.000000]; [0.000000, 1.000000]]
```


## Truthiness

Matrices are false only when their flat matrix storage is empty.


## Equality

General VM equality for matrices currently falls back to the matrix string form.


## Supported Operations

Matrix operations:

- `MAT_NEW`
- `MAT_GET`
- `MAT_SET`
- `MAT_ROWS`
- `MAT_COLS`
- `MAT_MUL`
- `MAT_ADD`
- `MAT_SUB`
- `MAT_SCALE`
- `MAT_IDENTITY`
- `MAT_TRACE`
- `MAT_RREF`
- `MAT_RANK`
- `MAT_EIGEN`
- `MAT_SOLVE`
- `MAT_TRANSPOSE`
- `MAT_DET`
- `MAT_INV`


## Example

```aic
.main
start:
  MAT_IDENTITY $0, #2
  PRINTLN $0
  HALT
```

Output:

```text
mat(2x2)[[1.000000, 0.000000]; [0.000000, 1.000000]]
```


## Errors

An error is raised when:

- a matrix operation receives a non-matrix where a matrix is required
- a flat index is negative or out of bounds
- matrix dimensions do not satisfy the operation
- square-matrix operations receive non-square matrices
- `MAT_EIGEN` receives a matrix that is not 2x2 or would produce complex eigenvalues
- `MAT_SOLVE` receives a singular matrix or a right-hand vector with the wrong size


## Notes

- Matrix elements are always stored as doubles.
- `MAT_INV` returns null for singular square matrices.
