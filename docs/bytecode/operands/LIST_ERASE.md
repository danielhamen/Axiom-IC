# `LIST_ERASE` Instruction

Removes an item from a list by index.


## Syntax

```
LIST_ERASE <list>, <index>
```

- `<list>` list slot to mutate (slot operand, usually `$n`)
- `<index>` non-negative index to remove (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Removes the element at `<index>`.
- Writes the shortened list back to `<list>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_ERASE $1, #0
  HALT
```


## Errors

An error is raised if:

- an index is negative or out of bounds


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
