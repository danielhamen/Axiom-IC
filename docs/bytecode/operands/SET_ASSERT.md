# `SET_ASSERT` Instruction

Asserts every set item matches a type expression.

## Category

- `set`

## Syntax

```aic
SET_ASSERT <set>, <type>
```

## Behavior

Continues when every item in `<set>` matches `<type>`.

## Errors

Raises an error if `<set>` is not a set, `<type>` is invalid, or any item does not match.
