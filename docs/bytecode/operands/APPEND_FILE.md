# `APPEND_FILE` Instruction

Appends a string to a file.



## Category

- `io`

## Syntax

```
APPEND_FILE <path>, <content>
```

- `<path>` string path to append (slot operand, usually `$n`)
- `<content>` string content to append (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Opens the path in binary append mode.
- Writes the content string at the end.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `io` operation category.


## Example

```
.main
start:
  STORE $1, #~"/tmp/aic-example.txt"
  STORE $2, #~"data"
  APPEND_FILE $1, $2
  HALT
```


## Errors

An error is raised if:

- the file cannot be opened


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
