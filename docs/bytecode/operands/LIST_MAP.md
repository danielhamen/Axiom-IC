# `LIST_MAP` Instruction

Maps a list through a function and stores a new list.

## Category

- `list`

## Syntax

```aic
LIST_MAP <dst>, <list>, <fn>
```

## Behavior

Calls `<fn>` once for each item with `(item, index)` arguments. The returned values are collected into `<dst>`.

## Errors

Raises an error if `<list>` is not a list or `<fn>` does not name a callable function.
