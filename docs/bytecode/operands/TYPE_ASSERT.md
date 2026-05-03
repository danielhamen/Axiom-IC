# `TYPE_ASSERT` Instruction

Requires a value to match a runtime type expression.


## Category

- `type`

## Syntax

```
TYPE_ASSERT <value>, <type>
```

- `<value>` value to validate
- `<type>` string type expression, such as `#~"Integer|Float"`


## Semantics

- Reads `<value>`.
- Reads `<type>` as a string.
- Raises an error if the value does not match the type expression.
- Continues to the next instruction when the value matches.


## Type Expressions

Type expressions support single types and unions:

```aic
#~"Integer"
#~"Integer|Float"
#~"Integer OR Float"
#~"Any"
```


## Example

```
.import <type>
.import <control_flow>

.main
start:
  TYPE_ASSERT #1, #~"Integer|Float"
  HALT
```


## Errors

An error is raised if:

- `<type>` is not a string
- `<type>` is an invalid type expression
- `<value>` does not match `<type>`


## Notes

- Use `TYPE_IS` when a boolean result is needed instead of an error.
