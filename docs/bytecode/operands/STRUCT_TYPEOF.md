# `STRUCT_TYPEOF` Instruction

Gets the declared structure type name for an instance.


## Syntax

```
STRUCT_TYPEOF <dst>, <obj>
```

- `<dst>` destination slot for the type name string (`$n`)
- `<obj>` structure instance


## Semantics

- Reads `<obj>` as a `Struct`.
- Creates a string containing the structure's type name.
- Stores the string in `<dst>`.


## Implementation Notes

- Reads `Value::struct_name`.
- The result is a normal `String` value.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  STRUCT_TYPEOF $2, $1
  PRINTLN $2
  HALT
```

### Output

```
Point
```


## Errors

An error is raised if:

- `<obj>` is not a `Struct`
- `<dst>` is not a writable slot


## Notes

- General `TYPEOF` returns the runtime kind `Struct`; `STRUCT_TYPEOF` returns the named structure type.
