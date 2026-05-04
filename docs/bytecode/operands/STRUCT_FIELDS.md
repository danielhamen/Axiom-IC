# `STRUCT_FIELDS` Instruction

Enumerates fields on a struct or struct definition.

## Category

- `struct`

## Syntax

```aic
STRUCT_FIELDS <dst>, <struct_or_def>
```

## Behavior

Stores a list of field names in `<dst>`.

## Errors

Raises an error if `<struct_or_def>` is not a struct or struct definition.
