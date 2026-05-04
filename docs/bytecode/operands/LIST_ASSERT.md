# `LIST_ASSERT` Instruction

Asserts every list item matches a type expression.

## Category

- `list`

## Syntax

```aic
LIST_ASSERT <list>, <type>
```

## Behavior

Continues when every item in `<list>` matches `<type>`.

## Errors

Raises an error if `<list>` is not a list, `<type>` is invalid, or any item does not match.
