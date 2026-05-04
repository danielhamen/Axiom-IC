# `STRUCT_DEF_FIELD_IMMUTABLE` Instruction

Marks a struct field as mutable or immutable.

## Category

- `struct`

## Syntax

```aic
STRUCT_DEF_FIELD_IMMUTABLE <def>, <name>, <bool>
```

## Behavior

Updates the field named `<name>` on `<def>` so assignments can be allowed or rejected after construction.

## Errors

Raises an error if `<def>` is not a mutable struct definition or the field is missing.
