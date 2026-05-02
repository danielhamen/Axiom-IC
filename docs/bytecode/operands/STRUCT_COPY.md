# `STRUCT_COPY` Instruction

Copies a structure instance.


## Syntax

```
STRUCT_COPY <dst>, <obj>
```

- `<dst>` destination slot for the copied structure (`$n`)
- `<obj>` structure instance to copy


## Semantics

- Reads `<obj>` as a `Struct`.
- Copies the structure value.
- Stores the copy in `<dst>`.


## Implementation Notes

- This is a value copy of the `Value` object.
- Field metadata and field values are copied.
- Nested values follow normal `Value` copy behavior.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  STRUCT_COPY $2, $1
  PRINTLN $2
  HALT
```


## Errors

An error is raised if:

- `<obj>` is not a `Struct`
- `<dst>` is not a writable slot


## Notes

- Mutating the copy does not mutate the original slot.
