# `STRUCT_STATIC_CALL` Instruction

Calls a static method on a struct definition or struct value.

## Category

- `struct`

## Syntax

```aic
STRUCT_STATIC_CALL <dst>, <struct_or_def>, <method>
```

## Behavior

Looks up `<method>` in the static method table, calls the target function without adding an implicit `self`, and stores the return value in `<dst>`.

Pending `ARG` and `KWARG` values are forwarded to the static method.

## Errors

Raises an error if `<struct_or_def>` is not a struct or struct definition, the static method is missing, or the target function cannot be called.
