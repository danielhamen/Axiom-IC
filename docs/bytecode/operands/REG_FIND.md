# `REG_FIND` Instruction

Finds the first regex match offset.

## Category

- `regex`

## Syntax

```
REG_FIND <dst>, <text>, <pattern>
```

## Behavior

Writes the zero-based start index of the first match, or `-1` when no match exists.
