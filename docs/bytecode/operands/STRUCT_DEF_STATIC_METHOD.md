# `STRUCT_DEF_STATIC_METHOD` Instruction

Attaches a static method to a struct definition.

## Category

- `struct`

## Syntax

```aic
STRUCT_DEF_STATIC_METHOD <def>, <method>, <fn>
```

## Behavior

Registers `<method>` on `<def>` and points it at function `<fn>`. Static methods are called with `STRUCT_STATIC_CALL` and do not receive an implicit `self` argument.

## Errors

Raises an error if `<def>` is not a mutable struct definition, `<method>` is empty, or `<fn>` is not callable.
