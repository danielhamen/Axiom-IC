# `SET_VALIDATE` Instruction

Validates every set item against a type expression.

## Category

- `set`

## Syntax

```aic
SET_VALIDATE <dst>, <set>, <type>
```

## Behavior

Stores `true` in `<dst>` when every item in `<set>` matches `<type>`, otherwise stores `false`.

## Errors

Raises an error if `<set>` is not a set or `<type>` is not a valid type expression.
