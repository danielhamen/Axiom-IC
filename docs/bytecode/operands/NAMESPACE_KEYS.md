# `NAMESPACE_KEYS` Instruction

Lists direct namespace members.

## Category

- `namespace`

## Syntax

```aic
NAMESPACE_KEYS <dst>, <namespace>
```

## Behavior

Writes a sorted list of direct value member names and function binding names to `<dst>`.

## Errors

Raises an error if `<namespace>` is not a namespace.
