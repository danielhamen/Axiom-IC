# Floats

`Float` is the VM's double-precision numeric type.


## Representation

- Runtime kind: `ValueKind::Float`
- Storage field: `Value::f`
- C++ storage type: `double`
- Type name returned by `TYPEOF`: `Float`


## Literals

Float immediates use `#` followed by a decimal literal:

```aic
STORE $0, #3.14
STORE $1, #-0.5
```

Float constants can also be declared in `.const`:

```aic
.const
  FLOAT 3.14
```

Or as an anonymous inline constant:

```aic
.const FLOAT 3.14159
```


## String Form

`Value::to_str()` formats floats with `std::to_string`, which prints six digits after the decimal by default.

```aic
.main
start:
  PRINTLN #3.5
  HALT
```

Output:

```text
3.500000
```


## Truthiness

Floats are false only when equal to `0.0`.

- `#0.0` is false
- any non-zero float is true

`NaN` is not equal to `0.0`, so it is truthy under the current implementation.


## Equality

Floats compare numerically with both floats and integers.

- `#1.0` equals `#1`
- `#1.5` does not equal `#1`


## Supported Operations

Floats are accepted by numeric operations such as:

- `ADD`, `SUB`, `MUL`, `DIV`
- `NEG`, `INC`, `DEC`, `ABS`
- numeric comparisons: `LT`, `LTE`, `GT`, `GTE`
- math functions such as `SIN`, `COS`, `SQRT`, `LOG`, `POW`
- `REM`, `FMOD`, `DEG2RAD`, `RAD2DEG`, `LERP`, and `MAP_RANGE`
- vector and matrix numeric element operations


## Casts

- `CAST_FLOAT` returns the same float value.
- `CAST_INT` truncates toward zero with `static_cast<int64_t>`.
- `CAST_BOOL` returns false only for `0.0`.
- `CAST_STRING` uses the float string form.


## Special Values

The VM exposes checks for floating special values:

- `IS_NAN`
- `IS_INF`

These checks return false for non-float values.


## Errors

An error is raised when:

- a numeric operation receives a non-numeric value
- division uses a zero divisor
- string-to-float casting fails
