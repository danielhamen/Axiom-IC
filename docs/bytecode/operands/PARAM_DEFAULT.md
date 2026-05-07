# `PARAM_DEFAULT` Instruction

Declares optional positional parameter metadata for overload resolution.

## Category

- `function`

## Syntax

```aic
PARAM_DEFAULT <index>, <type>, <default>
```

## Behavior

Records that positional argument `<index>` is optional and must match `<type>` when supplied. At runtime the instruction is a no-op after the function has been selected.

Use `ARG_GET_DEFAULT <dst>, <index>, <default>` in the function body to read the supplied argument or use the default value.

## Errors

Raises an error during parsing if `<index>` is not a non-negative integer immediate or `<type>` is not a string immediate.
