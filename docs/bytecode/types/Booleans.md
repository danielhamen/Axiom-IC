# Booleans

`Boolean` stores true or false values.


## Representation

- Runtime kind: `ValueKind::Boolean`
- Storage field: `Value::b`
- C++ storage type: `bool`
- Type name returned by `TYPEOF`: `Boolean`


## Literals

Boolean immediates use `#true` or `#false`:

```aic
STORE $0, #true
STORE $1, #false
```

Boolean constants can also be declared in `.const`:

```aic
.const
  BOOL true
```

Or as an anonymous inline constant:

```aic
.const BOOL true
```


## String Form

`Value::to_str()` returns `true` or `false`.


## Truthiness

Boolean truthiness is the stored boolean value.

- `#true` is true
- `#false` is false


## Equality

Booleans compare by exact boolean value.

- `#true` equals `#true`
- `#true` does not equal `#false`


## Supported Operations

Boolean-specific logical operations:

- `AND`
- `OR`
- `NOT`
- `XOR`

Conditional jumps and `CAST_BOOL` also use boolean values naturally.


## Casts

- `CAST_BOOL` returns the same boolean value.
- `CAST_INT` maps `true` to `1` and `false` to `0`.
- `CAST_FLOAT` maps `true` to `1.0` and `false` to `0.0`.
- `CAST_STRING` returns `true` or `false`.


## Errors

An error is raised when:

- boolean logical operations receive values that are neither both booleans nor both integers, depending on the operation


## Notes

- `AND`, `OR`, and `XOR` also support integer bitwise forms when both operands are integers.
- `NOT` also supports integer bitwise complement when the operand is an integer.
