# `SET_DIFFERENCE` Instruction

Computes the difference between two sets.

## Category

- `set`

## Syntax

```aic
SET_DIFFERENCE <dst>, <lhs>, <rhs>
```

## Behavior

Stores values that appear in `<lhs>` but not in `<rhs>`.

## Errors

Raises an error if either source operand is not a set.
