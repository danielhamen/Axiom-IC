# `STRUCT_IS` Instruction

Checks whether a structure instance has a specific structure type.


## Syntax

```
STRUCT_IS <dst>, <obj>, <type>
```

- `<dst>` destination slot for the boolean result (`$n`)
- `<obj>` structure instance
- `<type>` structure definition or string type name


## Semantics

- Reads `<obj>` as a `Struct`.
- Reads `<type>` as either:
  - a `StructDef`, using its name
  - a `String`, using the string contents
- Stores `true` in `<dst>` when the structure instance name equals the requested type name.
- Stores `false` otherwise.


## Implementation Notes

- The result is a normal `Boolean` value.
- A `StructDef` type operand must have a name.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  STRUCT_IS $2, $1, $0
  PRINTLN $2
  HALT
```

### Output

```
true
```


## Errors

An error is raised if:

- `<obj>` is not a `Struct`
- `<type>` is neither a `StructDef` nor a string
- `<type>` is an unnamed `StructDef`
- `<dst>` is not a writable slot


## Notes

- String type operands use normal immediate syntax, such as `#~"Point"`.
