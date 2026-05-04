# `STRUCT_DEF_EXTEND` Instruction

Copies fields, methods, and interface metadata from a parent struct definition.

## Category

- `struct`

## Syntax

```aic
STRUCT_DEF_EXTEND <def>, <parent_def>
```

## Behavior

Applies composition-style inheritance from `<parent_def>` into `<def>`.

## Errors

Raises an error if either operand is not a struct definition or `<def>` is sealed.
