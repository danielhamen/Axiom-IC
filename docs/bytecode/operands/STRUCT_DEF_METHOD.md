# `STRUCT_DEF_METHOD` Instruction

Attaches a method to a struct definition.

## Category

- `struct`

## Syntax

```aic
STRUCT_DEF_METHOD <def>, <method>, <fn>
```

## Behavior

Registers `<method>` on `<def>` and points it at function `<fn>`. Instance methods receive the struct instance as `self` when called with `STRUCT_CALL`.

## Errors

Raises an error if `<def>` is not a mutable struct definition or `<fn>` is not callable.
