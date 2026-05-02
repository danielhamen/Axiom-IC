# `DELETE_FILE` Instruction

Deletes a filesystem path if it exists.


## Syntax

```
DELETE_FILE <path>
```

- `<path>` string path to delete (slot operand, usually `$n`)


## Semantics

- Calls `std::filesystem::remove`.
- Ignores the remove result.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `io` operation category.


## Example

```
.main
start:
  STORE $1, #~"/tmp/aic-example.txt"
  STORE $2, #~"data"
  DELETE_FILE $1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
