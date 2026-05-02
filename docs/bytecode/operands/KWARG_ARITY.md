# `KWARG_ARITY` Instruction

Returns the number of keyword arguments in the current call frame.


## Syntax

```
KWARG_ARITY <dst>
```

- `<dst>` destination slot for the integer count


## Semantics

- Reads the active call frame
- Stores the number of keyword arguments as an integer


## Errors

An error is raised if:

- used outside of a called function
