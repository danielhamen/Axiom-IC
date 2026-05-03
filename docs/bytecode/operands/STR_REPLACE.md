# `STR_REPLACE` Instruction

Replaces all occurrences of a substring in a string slot.



## Category

- `string`

## Syntax

```
STR_REPLACE <target>, <from>, <to>
```

- `<target>` string slot to mutate (slot operand, usually `$n`)
- `<from>` substring to replace (immediate, slot, constant, argument, or supported operand value)
- `<to>` replacement string (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads and writes `<target>`.
- Does nothing when `<from>` is empty.
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
  STR_REPLACE $1, #~"", #~""
  HALT
```


## Errors

An error is raised if:

- a required operand is not a string


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
