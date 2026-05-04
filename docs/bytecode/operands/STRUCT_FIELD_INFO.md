# `STRUCT_FIELD_INFO` Instruction

Returns metadata for a struct field.

## Category

- `struct`

## Syntax

```aic
STRUCT_FIELD_INFO <dst>, <struct_or_def>, <field>
```

## Behavior

Stores a map describing the requested field, including type, visibility, mutability, and default metadata when available.

## Errors

Raises an error if the target is not a struct or struct definition, or if the field is missing.
