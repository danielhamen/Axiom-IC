# `NAMESPACE_NEW` Instruction

Creates a namespace value.

## Category

- `namespace`

## Syntax

```aic
NAMESPACE_NEW <dst>, <name>
```

## Behavior

Creates an empty namespace named `<name>` and stores it in `<dst>`.

## Errors

Raises an error if `<name>` is not a string.
