# `MAP_MERGE` Instruction

Merges two maps.

## Category

- `map`

## Syntax

```aic
MAP_MERGE <dst>, <lhs>, <rhs>
```

## Behavior

Stores a new map containing entries from `<lhs>` and `<rhs>`. Values from `<rhs>` replace matching keys from `<lhs>`.

## Errors

Raises an error if either source operand is not a map.
