# `TYPE_IS` Instruction

Tests whether a value matches a runtime type expression.


## Category

- `type`

## Syntax

```
TYPE_IS <dst>, <value>, <type>
```

- `<dst>` destination slot for the boolean result (`$n`)
- `<value>` value to test
- `<type>` string type expression, such as `#~"Integer|Float"`


## Semantics

- Reads `<value>`.
- Reads `<type>` as a string.
- Evaluates whether the value matches the type expression.
- Stores `true` or `false` in `<dst>`.


## Type Expressions

Type expressions support:

- single runtime types: `Integer`, `Float`, `String`
- aliases: `Number`, `Any`
- struct names: `Point`, `User`
- unions with `|`: `Integer|Float`
- unions with `OR`: `Integer OR Float`


## Example

```
.import <type>
.import <io>
.import <control_flow>

.main
start:
  TYPE_IS $0, #1.5, #~"Integer|Float"
  PRINTLN $0
  HALT
```

### Output

```
true
```


## Errors

An error is raised if:

- `<dst>` is not a writable slot
- `<type>` is not a string
- `<type>` is an invalid type expression


## Notes

- `TYPE_IS` does not mutate `<value>`.
