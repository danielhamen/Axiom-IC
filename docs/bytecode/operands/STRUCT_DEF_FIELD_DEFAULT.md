# `STRUCT_DEF_FIELD_DEFAULT` Instruction

Adds a typed field with a default value to an unsealed structure definition.


## Syntax

```
STRUCT_DEF_FIELD_DEFAULT <type>, <fieldName>, <fieldType>, <defaultValue>
```

- `<type>` structure definition slot to mutate (`$n`)
- `<fieldName>` string field name, usually `#~"y"`
- `<fieldType>` string type name, usually `#~"Integer"`
- `<defaultValue>` default runtime value for the field


## Semantics

- Reads `<type>` as an unsealed `StructDef`.
- Reads `<fieldName>` and `<fieldType>` as strings.
- Reads `<defaultValue>` as any runtime value.
- Verifies that `<defaultValue>` matches `<fieldType>`.
- Appends the field and default to the definition.
- Writes the updated definition back to `<type>`.


## Implementation Notes

- This is a four-operand instruction.
- Defaults are stored in `Value::struct_field_defaults`.
- Default presence is tracked in `Value::struct_field_has_defaults`.
- Instances copy defaults into `Value::struct_values` when constructed.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"y", #~"Integer", #7
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  PRINTLN $1
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
- `<defaultValue>` does not match `<fieldType>`
- `<type>` is not a writable slot


## Notes

- Use `#~"Any"` as the field type when any value type should be accepted.
