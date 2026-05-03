# `ARG_GET` Instruction

Reads a positional argument by dynamic index.



## Category

- `function`

## Syntax

```
ARG_GET <dst>, <index>
```

- `<dst>` destination slot for the argument value
- `<index>` non-negative integer argument index


## Semantics

- Reads the active call frame
- Stores positional argument `<index>` in `<dst>`


## Errors

An error is raised if:

- used outside of a called function
- `<index>` is negative or out of range


## Notes

- Static `arg0`, `arg1`, etc. operands still work for fixed-index positional access.
