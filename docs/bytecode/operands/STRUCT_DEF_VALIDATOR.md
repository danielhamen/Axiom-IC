# `STRUCT_DEF_VALIDATOR` Instruction

Attaches a constructor validation hook to a struct definition.

## Category

- `struct`

## Syntax

```aic
STRUCT_DEF_VALIDATOR <def>, <fn>
```

## Behavior

Registers `<fn>` as the validator called during struct construction.

## Errors

Raises an error if `<def>` is not a mutable struct definition or `<fn>` is not callable.
