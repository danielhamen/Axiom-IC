# `STRUCT_METHODS` Instruction

Enumerates methods on a struct or struct definition.

## Category

- `struct`

## Syntax

```aic
STRUCT_METHODS <dst>, <struct_or_def>
```

## Behavior

Stores a list of method names in `<dst>`.

## Errors

Raises an error if `<struct_or_def>` is not a struct or struct definition.
