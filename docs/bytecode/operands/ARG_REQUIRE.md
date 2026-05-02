# `ARG_REQUIRE` Instruction

Requires a positional argument to exist and match a runtime type.


## Syntax

```
ARG_REQUIRE <index>, <type>
```

- `<index>` non-negative integer argument index
- `<type>` string type name


## Semantics

- Reads positional argument `<index>`
- Raises an error if the argument is missing
- Raises an error if its runtime type does not match `<type>`


## Supported Type Names

- `Integer`
- `Float`
- `String`
- `Boolean`
- `Null`
- `List`
- `Map`
- `Set`
- `Vector`
- `Matrix`
- `Number` for either integer or float
- `Any` for any runtime value


## Errors

An error is raised if:

- used outside of a called function
- `<index>` is negative or out of range
- `<type>` is not a string
- the argument type does not match
