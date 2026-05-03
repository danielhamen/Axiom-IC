# `STR_SPLIT` Instruction

Splits a string into a list of strings.



## Category

- `string`

## Syntax

```
STR_SPLIT <dst>, <lhs>, <rhs>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<lhs>` left string (immediate, slot, constant, argument, or supported operand value)
- `<rhs>` right string (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Splits by `<delimiter>`.
- If the delimiter is empty, splits into one-character strings.
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
  STR_SPLIT $0, #1, #1
  HALT
```


## Errors

An error is raised if:

- a required operand is not a string


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
