# `LIST_CLEAR` Instruction

Removes all elements from a list.



## Category

- `list`

## Syntax

```
LIST_CLEAR <list>
```

- `<list>` list slot to clear (slot operand, usually `$n`)


## Semantics

- Clears the list in place.
- Writes the empty list back to `<list>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_CLEAR $1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
