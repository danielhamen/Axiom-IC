# `NAMESPACE_HAS` Instruction

Checks whether a namespace path exists.

## Category

- `namespace`

## Syntax

```aic
NAMESPACE_HAS <dst>, <namespace>, <path>
```

## Behavior

Writes `true` to `<dst>` if `<path>` exists as either a value member or function binding. Writes `false` otherwise.

## Errors

Raises an error if `<namespace>` is not a namespace or `<path>` is not a valid string path.
