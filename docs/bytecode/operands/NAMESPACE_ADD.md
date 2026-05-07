# `NAMESPACE_ADD` Instruction

Adds a value member to a namespace.

## Category

- `namespace`

## Syntax

```aic
NAMESPACE_ADD <namespace>, <path>, <value>
```

## Behavior

Stores `<value>` at `<path>` inside `<namespace>`. Paths are dot-separated, so `#~"Out.name"` writes `name` inside the nested `Out` namespace. Missing intermediate namespaces are created automatically.

## Errors

Raises an error if `<namespace>` is not a namespace, `<path>` is not a string, or an intermediate path segment already exists but is not a namespace.
