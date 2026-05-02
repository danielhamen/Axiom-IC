# `STRUCT_GET` Instruction

Reads a structure field by name.


## Syntax

```
STRUCT_GET <dst>, <obj>, <fieldName>
```

- `<dst>` destination slot for the field value (`$n`)
- `<obj>` structure instance
- `<fieldName>` string field name, usually `#~"x"`


## Semantics

- Reads `<obj>` as a `Struct`.
- Reads `<fieldName>` as a string.
- Finds the field by name.
- Stores the field value in `<dst>`.


## Implementation Notes

- Field lookup scans `Value::struct_field_names`.
- Field values are read from `Value::struct_values`.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"x", #~"Integer", #4
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  STRUCT_GET $2, $1, #~"x"
  PRINTLN $2
  HALT
```


## Errors

An error is raised if:

- `<obj>` is not a `Struct`
- `<fieldName>` is not a string
- the field does not exist
- `<dst>` is not a writable slot


## Notes

- For index-based access, use `STRUCT_GET_I`.
