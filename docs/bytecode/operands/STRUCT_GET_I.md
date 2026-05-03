# `STRUCT_GET_I` Instruction

Reads a structure field by index.



## Category

- `struct`

## Syntax

```
STRUCT_GET_I <dst>, <obj>, <index>
```

- `<dst>` destination slot for the field value (`$n`)
- `<obj>` structure instance
- `<index>` non-negative field index


## Semantics

- Reads `<obj>` as a `Struct`.
- Reads `<index>` as a non-negative integer.
- Stores the field value at `<index>` in `<dst>`.


## Implementation Notes

- Indexes follow definition field order.
- Values are read from `Value::struct_values`.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"x", #~"Integer", #4
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  STRUCT_GET_I $2, $1, #0
  PRINTLN $2
  HALT
```


## Errors

An error is raised if:

- `<obj>` is not a `Struct`
- `<index>` is not an integer
- `<index>` is negative
- `<index>` is out of bounds
- `<dst>` is not a writable slot


## Notes

- Index access is faster to write but less self-documenting than name access.
