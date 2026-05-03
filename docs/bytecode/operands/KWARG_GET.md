# `KWARG_GET` Instruction

Reads a keyword argument by string key.



## Category

- `function`

## Syntax

```
KWARG_GET <dst>, <key>
```

- `<dst>` destination slot for the keyword value
- `<key>` string key, normally `#~"name"`


## Semantics

- Reads the active call frame
- Stores the keyword argument value in `<dst>`


## Errors

An error is raised if:

- used outside of a called function
- `<key>` is not a string
- the keyword argument is missing
