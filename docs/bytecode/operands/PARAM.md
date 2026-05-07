# `PARAM` Instruction

Declares required positional parameter metadata for overload resolution.

## Category

- `function`

## Syntax

```aic
PARAM <index>, <type>
```

## Behavior

Records that positional argument `<index>` is required and must match `<type>` when resolving overloaded calls. At runtime the instruction is a no-op after the function has been selected.

Use `ARG_GET` or `ARG_GET_DEFAULT` to read the argument in the function body.

## Errors

Raises an error during parsing if `<index>` is not a non-negative integer immediate or `<type>` is not a string immediate.
