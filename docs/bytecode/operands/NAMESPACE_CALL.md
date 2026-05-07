# `NAMESPACE_CALL` Instruction

Calls a namespaced function binding.

## Category

- `namespace`

## Syntax

```aic
NAMESPACE_CALL <dst>, <namespace>, <path>
```

## Behavior

Looks up the function binding at `<path>`, calls it, and writes the return value to `<dst>`.

Pending `ARG` and `KWARG` values are forwarded to the function.

## Errors

Raises an error if `<namespace>` is not a namespace, `<path>` is not a string, or the function binding does not exist.
