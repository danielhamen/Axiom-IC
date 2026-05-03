# `LIST_FIND` Instruction

Finds a value in a list.



## Category

- `list`

## Syntax

```
LIST_FIND <dst>, <list>, <value>
```

- `<dst>` destination slot for the found index (destination slot, usually `$n`)
- `<list>` list to search (immediate, slot, constant, argument, or supported operand value)
- `<value>` value to find (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Searches from the beginning using VM equality.
- Stores the first index or `-1` if not found.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_FIND $0, $1, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
