# `REG_GROUPS` Instruction

Returns regex capture groups.

## Category

- `regex`

## Syntax

```
REG_GROUPS <dst>, <text>, <pattern>
```

## Behavior

Writes a list containing the full match followed by captured groups. Writes an empty list when no match exists.
