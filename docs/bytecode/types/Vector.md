# Vector

`Vector` is a one-dimensional numeric container.


## Representation

- Runtime kind: `ValueKind::Vector`
- Storage field: `Value::vec`
- C++ storage type: `std::vector<double>`
- Type name returned by `TYPEOF`: `Vector`


## Construction

Vectors are created with `VEC_NEW`:

```aic
VEC_NEW $0, #3
```

The size operand is a non-negative integer. Elements are initialized to `0.0`.


## String Form

Vectors print as `vec(...)`:

```text
vec(1.000000, 2.000000, 3.000000)
```


## Truthiness

Vectors are false only when empty.


## Equality

General VM equality for vectors currently falls back to the vector string form.


## Supported Operations

Vector operations:

- `VEC_NEW`
- `VEC_PUSH`
- `VEC_GET`
- `VEC_SET`
- `VEC_LEN`
- `VEC_ADD`
- `VEC_SUB`
- `VEC_SCALE`
- `VEC_DOT`
- `VEC_CROSS`
- `VEC_MAG`
- `VEC_NORM`
- `VEC_PROJECT`
- `VEC_REFLECT`
- `VEC_ANGLE`
- `VEC_DISTANCE`
- `VEC_LERP`


## Example

```aic
.main
start:
  VEC_NEW $0, #3
  VEC_SET $0, #0, #1
  VEC_SET $0, #1, #2
  VEC_SET $0, #2, #3
  PRINTLN $0
  HALT
```

Output:

```text
vec(1.000000, 2.000000, 3.000000)
```


## Errors

An error is raised when:

- a vector operation receives a non-vector where a vector is required
- an index is negative or out of bounds
- vector sizes do not match for operations that require equal sizes
- `VEC_CROSS` receives vectors that are not 3D
- normalization, projection, reflection, or angle operations receive invalid zero vectors


## Notes

- Vector elements are always stored as doubles.
- Numeric operands used with vector operations may be integers or floats.
