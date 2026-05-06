# `REG_MATCH` Instruction

Tests whether a regex pattern matches a string.

## Category

- `regex`

## Syntax

```
REG_MATCH <dst>, <text>, <pattern>
```

`<pattern>` is normally written with a regex string literal such as `#re"[0-9]+"`.

## Behavior

Writes `true` when `<pattern>` matches any part of `<text>`, otherwise `false`.
