# `WRITE_FILE` Instruction

Writes a string to a file, replacing existing content.


## Syntax

```
WRITE_FILE <path>, <content>
```

- `<path>` string path to write (slot operand, usually `$n`)
- `<content>` string content to write (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Opens the path in binary truncate mode.
- Writes the content string.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `io` operation category.


## Example

```
.main
start:
  STORE $1, #~"/tmp/aic-example.txt"
  STORE $2, #~"data"
  WRITE_FILE $1, $2
  HALT
```


## Errors

An error is raised if:

- the file cannot be opened


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
