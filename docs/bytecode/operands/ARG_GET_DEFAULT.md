# `ARG_GET_DEFAULT` Instruction

Reads a positional argument or falls back to a default value.

## Category

- `function`

## Syntax

```aic
ARG_GET_DEFAULT <dst>, <index>, <default>
```

## Behavior

If positional argument `<index>` exists in the current call frame, writes it to `<dst>`. Otherwise writes `<default>` to `<dst>`.

## Errors

Raises an error if used outside a called function or if `<index>` is not a non-negative integer.
