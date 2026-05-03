# Null

`Null` represents absence of a value.


## Representation

- Runtime kind: `ValueKind::Null`
- No payload field is used
- Type name returned by `TYPEOF`: `Null`


## Literals

Null immediates use `#null` or `#NULL`:

```aic
STORE $0, #null
```

Null constants can also be declared in `.const`:

```aic
.const
  NULL
```

Or as an anonymous inline constant:

```aic
.const NULL
.const 3 NULL ; available as @3
```


## String Form

`Value::to_str()` returns `null`.


## Truthiness

Null is always false.


## Equality

Null equals null.

Null does not equal any non-null value.


## Supported Operations

Null can be:

- stored in slots
- placed in lists, maps, and sets
- printed
- tested with `IS_NULL`
- converted with `CAST_BOOL` and `CAST_STRING`


## Casts

- `CAST_BOOL` returns `false`.
- `CAST_STRING` returns `null`.
- `CAST_INT` and `CAST_FLOAT` are unsupported for null.


## Errors

An error is raised when:

- a typed operation receives null where another type is required
- `CAST_INT` or `CAST_FLOAT` is applied to null


## Notes

- `LIST_NEW` initializes every element to null.
- `MAT_INV` returns null when a square matrix is singular.
