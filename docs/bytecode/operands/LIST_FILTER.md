# `LIST_FILTER` Instruction

Filters a list through a predicate function.

## Category

- `list`

## Syntax

```aic
LIST_FILTER <dst>, <list>, <fn>
```

## Behavior

Calls `<fn>` once for each item with `(item, index)` arguments. Truthy results keep the original item in `<dst>`.

## Errors

Raises an error if `<list>` is not a list or `<fn>` does not name a callable function.
