# `ARG_ARITY` Instruction

Returns the number of positional arguments in the current call frame.



## Category

- `function`

## Syntax

```
ARG_ARITY <dst>
```

- `<dst>` destination slot for the integer count


## Semantics

- Reads the active call frame
- Stores the number of positional arguments as an integer


## Errors

An error is raised if:

- used outside of a called function
