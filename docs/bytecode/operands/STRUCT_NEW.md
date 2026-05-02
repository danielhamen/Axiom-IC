# `STRUCT_NEW` Instruction

Creates a new structure instance from a sealed definition.


## Syntax

```
STRUCT_NEW <dst>, <type>
```

- `<dst>` destination slot for the new structure instance (`$n`)
- `<type>` sealed structure definition


## Semantics

- Reads `<type>` as a `StructDef`.
- Verifies that the definition is sealed and named.
- Creates a `Struct` instance with the definition's name and fields.
- Initializes fields with their defaults, or `null` when no default exists.
- Stores the instance in `<dst>`.


## Implementation Notes

- Runtime kind: `ValueKind::Struct`
- Instance values are stored in `Value::struct_values`.
- Field names and type names are copied from the definition onto the instance.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"x", #~"Integer", #0
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  PRINTLN $1
  HALT
```


## Errors

An error is raised if:

- `<type>` is not a `StructDef`
- `<type>` is not sealed
- `<type>` has no name
- `<dst>` is not a writable slot


## Notes

- Use `STRUCT_INIT` when initial field values should be supplied at construction time.
