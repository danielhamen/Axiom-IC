# `LIST_VALIDATE` Instruction

Validates every list item against a type expression.

## Category

- `list`

## Syntax

```aic
LIST_VALIDATE <dst>, <list>, <type>
```

## Behavior

Stores `true` in `<dst>` when every item in `<list>` matches `<type>`, otherwise stores `false`.

## Errors

Raises an error if `<list>` is not a list or `<type>` is not a valid type expression.
