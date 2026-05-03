# `STR_LOWER` Instruction

Converts a string to lowercase.



## Category

- `string`

## Syntax

```
STR_LOWER <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source string (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Applies `tolower` to each byte.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `string` operation category.
- String operations use `std::string` byte indexing.


## Example

```
.main
start:
  STORE $1, #~"hello"
  STORE $2, #~"l"
  STR_LOWER $0, $1
  HALT
```


## Errors

An error is raised if:

- a required operand is not a string


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
