# `STRUCT_DEF_NEW` Instruction

Creates a new mutable structure definition.



## Category

- `struct`

## Syntax

```
STRUCT_DEF_NEW <dst>
```

- `<dst>` destination slot for the new structure definition (`$n`)


## Semantics

- Allocates an empty `StructDef` value.
- The definition has no name, no fields, and is not sealed.
- Stores the definition in `<dst>`.


## Implementation Notes

- Runtime kind: `ValueKind::StructDef`
- A definition can be modified until `STRUCT_DEF_SEAL` is executed.
- A definition cannot be instantiated until it is sealed.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  PRINTLN $0
  HALT
```


## Errors

An error is raised if:

- `<dst>` is not a writable slot


## Notes

- Use `STRUCT_DEF_NAME` and `STRUCT_DEF_FIELD` to complete the definition before sealing it.
