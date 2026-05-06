# `JSON_STR_DUMP` Instruction

Serializes an AIC value to a JSON string.

## Category

- `json`

## Syntax

```
JSON_STR_DUMP <dst>, <value>
```

## Behavior

Writes a JSON string representation of `<value>` to `<dst>`. Supports nulls, booleans, integers, floats, strings, lists, and maps.
