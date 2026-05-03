# `STRUCT_DEF_FIELD` Instruction

Adds a typed field without a default value to an unsealed structure definition.



## Category

- `struct`

## Syntax

```
STRUCT_DEF_FIELD <type>, <fieldName>, <fieldType>
```

- `<type>` structure definition slot to mutate (`$n`)
- `<fieldName>` string field name, usually `#~"x"`
- `<fieldType>` string type name, usually `#~"Integer"`


## Semantics

- Reads `<type>` as an unsealed `StructDef`.
- Reads `<fieldName>` and `<fieldType>` as strings.
- Appends the field to the definition.
- Records that the field has no default value.
- Writes the updated definition back to `<type>`.


## Implementation Notes

- Field order is significant.
- Field names are stored in `Value::struct_field_names`.
- Field type names are stored in `Value::struct_field_types`.
- Instances created from this definition initialize fields without defaults to `null`.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD $0, #~"x", #~"Integer"
  STRUCT_DEF_SEAL $0
  HALT
```


## Errors

An error is raised if:

- `<type>` is not a `StructDef`
- `<type>` is already sealed
- `<fieldName>` is not a string
- `<fieldType>` is not a string
- `<fieldName>` is empty
- `<fieldType>` is empty
- the field name already exists on the definition
- `<type>` is not a writable slot


## Notes

- Type names are checked when values are supplied by `STRUCT_INIT`, `STRUCT_SET`, or `STRUCT_SET_I`.
