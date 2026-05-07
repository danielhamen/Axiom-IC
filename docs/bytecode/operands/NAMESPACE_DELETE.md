# `NAMESPACE_DELETE` Instruction

Removes a namespace member.

## Category

- `namespace`

## Syntax

```aic
NAMESPACE_DELETE <namespace>, <path>
```

## Behavior

Removes the value member or function binding at `<path>`.

## Errors

Raises an error if `<namespace>` is not a namespace, `<path>` is not a string, or an intermediate path segment is missing.
