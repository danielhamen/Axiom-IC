# `INTERFACE_HAS` Instruction

Checks whether an interface declares a method.

## Category

- `interface`

## Syntax

```aic
INTERFACE_HAS <dst>, <interface>, <method>
```

## Behavior

Stores `true` in `<dst>` when `<interface>` requires `<method>`, otherwise stores `false`.

## Errors

Raises an error if `<interface>` is not an interface or `<method>` is not a string.
