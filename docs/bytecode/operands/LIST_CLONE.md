# `LIST_CLONE` Instruction

Copies a list value.

## Category

- `list`

## Syntax

```aic
LIST_CLONE <dst>, <list>
```

## Behavior

Stores a copy of `<list>` in `<dst>`.

## Errors

Raises an error if `<list>` is not a list.
