# `STRUCT_IMPLEMENTS` Instruction

Checks whether a struct or definition implements an interface.

## Category

- `struct`

## Syntax

```aic
STRUCT_IMPLEMENTS <dst>, <struct_or_def>, <interface>
```

## Behavior

Stores `true` when the target declares or satisfies `<interface>`.

## Errors

Raises an error if the target is not a struct or struct definition.
