# `STRUCT_DEF_FIELD_VISIBILITY` Instruction

Sets field visibility metadata on a struct definition.

## Category

- `struct`

## Syntax

```aic
STRUCT_DEF_FIELD_VISIBILITY <def>, <name>, <visibility>
```

## Behavior

Updates the field named `<name>` on `<def>` to use `<visibility>`.

## Errors

Raises an error if `<def>` is not a mutable struct definition, the field is missing, or visibility is invalid.
