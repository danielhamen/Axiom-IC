# `KWARG_HAS` Instruction

Checks whether the current call frame has a keyword argument.



## Category

- `function`

## Syntax

```
KWARG_HAS <dst>, <key>
```

- `<dst>` destination slot for the boolean result
- `<key>` string key, normally `#~"name"`


## Semantics

- Reads the active call frame
- Stores `true` when the keyword argument exists, otherwise stores `false`


## Errors

An error is raised if:

- used outside of a called function
- `<key>` is not a string
