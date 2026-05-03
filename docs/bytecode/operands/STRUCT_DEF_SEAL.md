# `STRUCT_DEF_SEAL` Instruction

Seals a structure definition so it can be instantiated.



## Category

- `struct`

## Syntax

```
STRUCT_DEF_SEAL <type>
```

- `<type>` structure definition slot to seal (`$n`)


## Semantics

- Reads `<type>` as a `StructDef`.
- Verifies that the definition has a name.
- Marks the definition as sealed.
- Writes the sealed definition back to `<type>`.


## Implementation Notes

- Sealed state is stored in `Value::struct_sealed`.
- Sealed definitions cannot be renamed or have fields added.
- `STRUCT_NEW` and `STRUCT_INIT` reject unsealed definitions.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  HALT
```


## Errors

An error is raised if:

- `<type>` is not a `StructDef`
- the definition has no name
- `<type>` is not a writable slot


## Notes

- Sealing a definition with zero fields is allowed.
