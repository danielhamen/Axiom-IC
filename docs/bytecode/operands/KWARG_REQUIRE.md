# `KWARG_REQUIRE` Instruction

Requires a keyword argument to exist and match a runtime type.



## Category

- `function`

## Syntax

```
KWARG_REQUIRE <key>, <type>
```

- `<key>` string keyword name
- `<type>` string type name


## Semantics

- Reads keyword argument `<key>`
- Raises an error if the keyword argument is missing
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
- `<key>` is not a string
- `<type>` is not a string
- the keyword argument is missing
- the keyword argument type does not match
