# `STRUCT_CALL` Instruction

Calls a method on a struct value.

## Category

- `struct`

## Syntax

```aic
STRUCT_CALL <dst>, <struct>, <method>
```

## Behavior

Looks up `<method>` on `<struct>`, calls the target function with the struct value as `self`, and stores the return value in `<dst>`.

Inside the callee, the receiver is available through `SELF <dst>` or `ARG_GET <dst>, #0`.

Pending `ARG` and `KWARG` values are forwarded after the implicit receiver.

## Errors

Raises an error if `<struct>` is not a struct or the method is missing.
