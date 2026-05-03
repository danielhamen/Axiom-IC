# Integers

`Integer` is the VM's signed whole-number type.


## Representation

- Runtime kind: `ValueKind::Integer`
- Storage field: `Value::i`
- C++ storage type: `int64_t`
- Type name returned by `TYPEOF`: `Integer`


## Literals

Integer immediates use `#` followed by an integer literal:

```aic
STORE $0, #42
STORE $1, #-7
```

Integer constants can also be declared in `.const`:

```aic
.const
  INT 42
```

Or as an anonymous inline constant:

```aic
.const INT 42
.const 10 INT 99 ; available as @10
```


## String Form

`Value::to_str()` formats integers with `std::to_string`.

```aic
.main
start:
  PRINTLN #42
  HALT
```

Output:

```text
42
```


## Truthiness

Integers are false only when equal to `0`.

- `#0` is false
- any non-zero integer is true

`CAST_BOOL` uses this rule.


## Equality

Integers compare numerically with both integers and floats.

- `#1` equals `#1`
- `#1` equals `#1.0`
- `#1` does not equal `#2`

This equality rule is also used by list search and set membership.


## Supported Operations

Integer values are accepted by numeric operations such as:

- `ADD`, `SUB`, `MUL`, `DIV`
- `NEG`, `INC`, `DEC`, `ABS`
- numeric comparisons: `LT`, `LTE`, `GT`, `GTE`
- math functions that accept numeric operands

Integer-only operations:

- `MOD`
- `GCD`
- `LCM`
- `FACTORIAL`
- integer bitwise forms of `AND`, `OR`, `NOT`, and `XOR`
- `RAND_INT` bounds


## Casts

- `CAST_INT` returns the same integer value.
- `CAST_FLOAT` converts to a double.
- `CAST_BOOL` returns false for `0`, true otherwise.
- `CAST_STRING` uses the integer string form.


## Errors

An error is raised when:

- an integer-only operation receives a non-integer
- division or modulo uses a zero divisor
- an integer literal is out of range for `int64_t`


## Notes

- Integer arithmetic between two integers keeps an integer result for `ADD`, `SUB`, `MUL`, and `DIV`.
- `DIV` between two integers uses integer division.
