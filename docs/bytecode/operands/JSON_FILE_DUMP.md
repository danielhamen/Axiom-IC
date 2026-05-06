# `JSON_FILE_DUMP` Instruction

Serializes an AIC value to a JSON file.

## Category

- `json`

## Syntax

```
JSON_FILE_DUMP <path>, <value>
```

## Behavior

Writes a JSON representation of `<value>` to the file at `<path>`. Supports nulls, booleans, integers, floats, strings, lists, and maps.
