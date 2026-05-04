# `STRUCT_COMPOSE` Instruction

Composes two struct definitions.

## Category

- `struct`

## Syntax

```aic
STRUCT_COMPOSE <dst>, <lhs>, <rhs>
```

## Behavior

Stores a struct definition containing compatible metadata from both source definitions.

## Errors

Raises an error if either source operand is not a struct definition or fields conflict.
