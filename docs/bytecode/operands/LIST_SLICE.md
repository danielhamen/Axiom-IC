# `LIST_SLICE` Instruction

Copies a suffix of a list.


## Syntax

```
LIST_SLICE <dst>, <list>, <start>
```

- `<dst>` destination slot for the sliced list (destination slot, usually `$n`)
- `<list>` source list (immediate, slot, constant, argument, or supported operand value)
- `<start>` non-negative start index (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Copies elements from `<start>` through the end of `<list>`.
- Stores the copied list in `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_SLICE $0, $1, #0
  HALT
```


## Errors

An error is raised if:

- an index is negative or out of bounds


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
