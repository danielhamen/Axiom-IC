# `KWARG` Instruction

Sets one pending keyword argument for the next `CALL`.


## Syntax

```
KWARG <key>, <value>
```

- `<key>` string key, normally `#~"name"`
- `<value>` value to pass under that key


## Semantics

- Reads `<key>` as a string
- Reads `<value>`
- Stores the value in the pending keyword argument map
- The next `CALL` consumes pending keyword arguments into the callee call frame


## Errors

An error is raised if:

- `<key>` is not a string
- the same pending keyword key is set more than once before `CALL`


## Notes

- Keyword arguments are accessed inside the callee with `KWARG_GET`, `KWARG_HAS`, `KWARG_ARITY`, and `KWARG_REQUIRE`.
