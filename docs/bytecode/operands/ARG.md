# `ARG` Instruction

Appends one pending positional argument for the next `CALL`.



## Category

- `function`

## Syntax

```
ARG <value>
```

- `<value>` value to pass to the next function call


## Semantics

- Reads `<value>`
- Appends it to the pending positional argument list
- The next `CALL` consumes the pending arguments into the callee call frame


## Notes

- `ARG` is preferred over using `PUSH` for new function-call bytecode.
- Pending arguments are cleared when consumed by a successful `CALL`.
