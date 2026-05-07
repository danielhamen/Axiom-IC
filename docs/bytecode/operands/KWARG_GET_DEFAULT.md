# `KWARG_GET_DEFAULT` Instruction

Reads a keyword argument or falls back to a default value.

## Category

- `function`

## Syntax

```aic
KWARG_GET_DEFAULT <dst>, <key>, <default>
```

## Behavior

If keyword argument `<key>` exists in the current call frame, writes it to `<dst>`. Otherwise writes `<default>` to `<dst>`.

## Errors

Raises an error if used outside a called function or if `<key>` is not a string.
