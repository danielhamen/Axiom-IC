# `STRUCT_INIT` Instruction

Creates a new structure instance from a sealed definition and positional field values.



## Category

- `struct`

## Syntax

```
STRUCT_INIT <dst>, <type>, <v1>, <v2>, ...
```

- `<dst>` destination slot for the new structure instance (`$n`)
- `<type>` sealed structure definition
- `<v1>, <v2>, ...` optional positional field values


## Semantics

- Reads `<type>` as a sealed `StructDef`.
- Creates an instance using the same initialization behavior as `STRUCT_NEW`.
- Assigns each supplied value to the corresponding field by field order.
- Verifies each supplied value against that field's declared type.
- Stores the initialized instance in `<dst>`.


## Implementation Notes

- This is a variadic instruction.
- At least `<dst>` and `<type>` are required.
- Supplied values are assigned from index `0` upward.
- Unspecified fields keep their default value or `null`.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD $0, #~"x", #~"Integer"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"y", #~"Integer", #7
  STRUCT_DEF_SEAL $0
  STRUCT_INIT $1, $0, #3
  PRINTLN $1
  HALT
```

### Output

```
Point{x: 3, y: 7}
```


## Errors

An error is raised if:

- fewer than two operands are supplied
- `<type>` is not a `StructDef`
- `<type>` is not sealed
- `<type>` has no name
- more values are supplied than the definition has fields
- any supplied value does not match its field type
- `<dst>` is not a writable slot


## Notes

- Field assignment is positional; use `STRUCT_SET` after construction for name-based assignment.
