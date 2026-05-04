# `MAP_ENTRIES` Instruction

Converts a map into entry pairs.

## Category

- `map`

## Syntax

```aic
MAP_ENTRIES <dst>, <map>
```

## Behavior

Stores a list of two-item lists. Each entry contains the string key and its value.

## Errors

Raises an error if `<map>` is not a map.
