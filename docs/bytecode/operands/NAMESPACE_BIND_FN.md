# `NAMESPACE_BIND_FN` Instruction

Binds a function under a namespace path.

## Category

- `namespace`

## Syntax

```aic
NAMESPACE_BIND_FN <namespace>, <path>, <fn>
```

## Behavior

Stores a function binding at `<path>`. Missing intermediate namespaces are created automatically.

Function bindings are invoked with `NAMESPACE_CALL`.

## Errors

Raises an error if `<namespace>` is not a namespace, `<path>` is not a string, or `<fn>` does not name an existing function.
