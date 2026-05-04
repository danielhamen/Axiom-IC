# `STRUCT_DEF_IMPLEMENT` Instruction

Declares that a struct definition implements an interface.

## Category

- `struct`

## Syntax

```aic
STRUCT_DEF_IMPLEMENT <def>, <interface>
```

## Behavior

Records `<interface>` on `<def>` after checking that required methods are available.

## Errors

Raises an error if `<def>` is not a mutable struct definition or interface requirements are not satisfied.
