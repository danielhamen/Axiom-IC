# `FILE_EXISTS` Instruction

Tests whether a filesystem path exists.



## Category

- `io`

## Syntax

```
FILE_EXISTS <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Stores a boolean result from `std::filesystem::exists`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `io` operation category.


## Example

```
.main
start:
  STORE $1, #~"/tmp/aic-example.txt"
  STORE $2, #~"data"
  FILE_EXISTS $0, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
