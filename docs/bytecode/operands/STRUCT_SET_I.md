# `STRUCT_SET_I` Instruction

Writes a structure field by index.



## Category

- `struct`

## Syntax

```
STRUCT_SET_I <obj>, <index>, <value>
```

- `<obj>` structure instance slot to mutate (`$n`)
- `<index>` non-negative field index
- `<value>` value to store


## Semantics

- Reads `<obj>` as a `Struct`.
- Reads `<index>` as a non-negative integer.
- Verifies that `<value>` matches the indexed field's declared type.
- Stores `<value>` into that field.
- Writes the updated structure back to `<obj>`.


## Implementation Notes

- Indexes follow definition field order.
- Type names are read from `Value::struct_field_types`.
- Values are stored in `Value::struct_values`.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"x", #~"Integer", #0
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  STRUCT_SET_I $1, #0, #9
  PRINTLN $1
  HALT
```


## Errors

An error is raised if:

- `<obj>` is not a `Struct`
- `<index>` is not an integer
- `<index>` is negative
- `<index>` is out of bounds
- `<value>` does not match the field type
- `<obj>` is not a writable slot


## Notes

- Use `STRUCT_SET` when field names should remain visible in bytecode.
