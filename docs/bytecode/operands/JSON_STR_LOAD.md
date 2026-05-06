# `JSON_STR_LOAD` Instruction

Parses a JSON string into AIC values.

## Category

- `json`

## Syntax

```
JSON_STR_LOAD <dst>, <json>
```

## Behavior

Parses `<json>` as JSON and writes the result to `<dst>`. JSON objects become maps, arrays become lists, strings become strings, numbers become integers or floats, booleans become booleans, and `null` becomes null.
