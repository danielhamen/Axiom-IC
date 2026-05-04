# `STRUCT_DESTRUCTURE` Instruction

Destructures struct fields into destination operands.

## Category

- `struct`

## Syntax

```aic
STRUCT_DESTRUCTURE <struct>, <dst>...
```

## Behavior

Writes field values from `<struct>` into the destination operands in field declaration order.

## Errors

Raises an error if the source is not a struct or there are more destinations than fields.
