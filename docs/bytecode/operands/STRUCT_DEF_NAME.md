# `STRUCT_DEF_NAME` Instruction

Assigns a name to an unsealed structure definition.


## Syntax

```
STRUCT_DEF_NAME <type>, <name>
```

- `<type>` structure definition slot to mutate (`$n`)
- `<name>` string name, usually an immediate such as `#~"Point"`


## Semantics

- Reads `<type>` as a `StructDef`.
- Reads `<name>` as a string.
- Stores the name on the structure definition.
- Writes the updated definition back to `<type>`.


## Implementation Notes

- Names are stored in `Value::struct_name`.
- The VM requires a non-empty name before sealing and instantiation.
- The instruction mutates by value: it reads the definition, modifies it, and writes it back.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  HALT
```


## Errors

An error is raised if:

- `<type>` is not a `StructDef`
- `<type>` is already sealed
- `<name>` is not a string
- `<name>` is empty
- `<type>` is not a writable slot


## Notes

- Bare quoted strings are not valid operands; use `#~"..."` for string immediates.
