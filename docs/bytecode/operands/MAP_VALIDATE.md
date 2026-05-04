# `MAP_VALIDATE` Instruction

Validates map keys and values against type expressions.

## Category

- `map`

## Syntax

```aic
MAP_VALIDATE <dst>, <map>, <key_type>, <value_type>
```

## Behavior

Stores `true` when every key matches `<key_type>` and every value matches `<value_type>`.

## Errors

Raises an error if `<map>` is not a map or either type expression is invalid.
