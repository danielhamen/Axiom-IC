# `SET_SYMMETRIC_DIFF` Instruction

Computes the symmetric difference between two sets.

## Category

- `set`

## Syntax

```aic
SET_SYMMETRIC_DIFF <dst>, <lhs>, <rhs>
```

## Behavior

Stores values that appear in exactly one of the two source sets.

## Errors

Raises an error if either source operand is not a set.
