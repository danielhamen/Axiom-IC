# `SELF` Instruction

Reads the receiver for the current instance method call.

## Category

- `function`

## Syntax

```aic
SELF <dst>
```

## Behavior

Copies the current method receiver into `<dst>`. `SELF` is equivalent to reading `arg0` during a `STRUCT_CALL`, but it validates that `arg0` is a struct and gives clearer errors.

## Errors

Raises an error if there is no active call frame, no receiver argument, or `arg0` is not a struct.
