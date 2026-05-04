# `LIST_DESTRUCTURE` Instruction

Destructures list items into destination operands.

## Category

- `list`

## Syntax

```aic
LIST_DESTRUCTURE <list>, <dst>...
```

## Behavior

Writes each requested list item to the corresponding destination operand.

## Errors

Raises an error if `<list>` is not a list or there are more destinations than list values.
