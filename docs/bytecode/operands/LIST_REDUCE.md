# `LIST_REDUCE` Instruction

Reduces a list into a single accumulator value.

## Category

- `list`

## Syntax

```aic
LIST_REDUCE <dst>, <list>, <fn>, <initial>
```

## Behavior

Starts with `<initial>` and calls `<fn>` for each item with `(accumulator, item, index)`. The final accumulator is stored in `<dst>`.

## Errors

Raises an error if `<list>` is not a list or `<fn>` does not name a callable function.
