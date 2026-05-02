# `STRUCT_SET` Instruction

Writes a structure field by name.


## Syntax

```
STRUCT_SET <obj>, <fieldName>, <value>
```

- `<obj>` structure instance slot to mutate (`$n`)
- `<fieldName>` string field name, usually `#~"x"`
- `<value>` value to store


## Semantics

- Reads `<obj>` as a `Struct`.
- Reads `<fieldName>` as a string.
- Finds the field by name.
- Verifies that `<value>` matches the field's declared type.
- Stores `<value>` into the field.
- Writes the updated structure back to `<obj>`.


## Implementation Notes

- The structure is updated by value and written back to the slot.
- Type validation uses the same type-name matching as function argument requirements.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"x", #~"Integer", #0
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  STRUCT_SET $1, #~"x", #9
  PRINTLN $1
  HALT
```


## Errors

An error is raised if:

- `<obj>` is not a `Struct`
- `<fieldName>` is not a string
- the field does not exist
- `<value>` does not match the field type
- `<obj>` is not a writable slot


## Notes

- Use `STRUCT_SET_I` for index-based writes.
