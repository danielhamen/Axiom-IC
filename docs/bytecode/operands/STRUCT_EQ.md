# `STRUCT_EQ` Instruction

Compares two structure instances for equality.



## Category

- `struct`

## Syntax

```
STRUCT_EQ <dst>, <a>, <b>
```

- `<dst>` destination slot for the boolean result (`$n`)
- `<a>` first structure instance
- `<b>` second structure instance


## Semantics

- Reads `<a>` and `<b>` as `Struct` values.
- Compares them using VM value equality.
- Stores the boolean result in `<dst>`.


## Implementation Notes

- `STRUCT_EQ` currently delegates to VM `value_equals`.
- For structures, general VM equality falls back to the string form produced by `to_str()`.
- Two structures with the same type name, field names, and printed field values compare equal.


## Example

```
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"x", #~"Integer", #1
  STRUCT_DEF_SEAL $0
  STRUCT_NEW $1, $0
  STRUCT_COPY $2, $1
  STRUCT_EQ $3, $1, $2
  PRINTLN $3
  HALT
```

### Output

```
true
```


## Errors

An error is raised if:

- `<a>` is not a `Struct`
- `<b>` is not a `Struct`
- `<dst>` is not a writable slot


## Notes

- Numeric field values use the VM's normal equality behavior only when compared directly; structure equality currently uses the full structure string form.
