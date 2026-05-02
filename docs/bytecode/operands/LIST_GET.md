# `LIST_GET` Instruction

Reads a list element by index.


## Syntax

```
LIST_GET <dst>, <list>, <index>
```

- `<dst>` destination slot for the element (destination slot, usually `$n`)
- `<list>` source list (immediate, slot, constant, argument, or supported operand value)
- `<index>` non-negative element index (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Stores `<list>[<index>]` in `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_GET $0, $1, #0
  HALT
```


## Errors

An error is raised if:

- an index is negative or out of bounds


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
