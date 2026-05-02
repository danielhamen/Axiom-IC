# `READ_FILE` Instruction

Reads a file into a string.


## Syntax

```
READ_FILE <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Opens the path in binary mode.
- Stores the full file contents as a string.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `io` operation category.


## Example

```
.main
start:
  STORE $1, #~"/tmp/aic-example.txt"
  STORE $2, #~"data"
  READ_FILE $0, #1
  HALT
```


## Errors

An error is raised if:

- the file cannot be opened


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
