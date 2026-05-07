# `NAMESPACE_GET` Instruction

Reads a value member from a namespace.

## Category

- `namespace`

## Syntax

```aic
NAMESPACE_GET <dst>, <namespace>, <path>
```

## Behavior

Reads the value stored at `<path>` and writes it to `<dst>`. Function bindings are called with `NAMESPACE_CALL` rather than returned by `NAMESPACE_GET`.

## Errors

Raises an error if `<namespace>` is not a namespace, `<path>` is not a string, or the value path does not exist.
