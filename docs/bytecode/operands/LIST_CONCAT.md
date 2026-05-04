# `LIST_CONCAT` Instruction

Concatenates two lists.

## Category

- `list`

## Syntax

```aic
LIST_CONCAT <dst>, <lhs>, <rhs>
```

## Behavior

Stores a new list containing all values from `<lhs>` followed by all values from `<rhs>`.

## Errors

Raises an error if either source operand is not a list.
