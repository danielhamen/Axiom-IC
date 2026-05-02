# `LIST_POP` Instruction

Removes the last item from a list.


## Syntax

```
LIST_POP <dst>, <list>
```

- `<dst>` destination for the popped value (destination slot, usually `$n`)
- `<list>` list slot to mutate (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads `<list>` as a list.
- Removes the final element.
- Stores the removed element in `<dst>` and writes the shortened list back.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_POP $0, $1
  HALT
```


## Errors

An error is raised if:

- the list is empty


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
