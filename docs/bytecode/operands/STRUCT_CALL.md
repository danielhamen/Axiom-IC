# `STRUCT_CALL` Instruction

Calls a method on a struct value.

## Category

- `struct`

## Syntax

```aic
STRUCT_CALL <dst>, <struct>, <method>
```

## Behavior

Looks up `<method>` on `<struct>`, calls the target function with the struct value, and stores the return value in `<dst>`.

## Errors

Raises an error if `<struct>` is not a struct or the method is missing.
