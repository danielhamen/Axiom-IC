# `STRUCT_INTERFACES` Instruction

Enumerates interfaces declared by a struct or struct definition.

## Category

- `struct`

## Syntax

```aic
STRUCT_INTERFACES <dst>, <struct_or_def>
```

## Behavior

Stores a list of interface names in `<dst>`.

## Errors

Raises an error if `<struct_or_def>` is not a struct or struct definition.
