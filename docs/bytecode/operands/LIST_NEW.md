# `LIST_NEW` Instruction

Creates a new list with a fixed initial size.


## Syntax

```
LIST_NEW <dst>, <size>
```

- `<dst>` destination slot for the new list (destination slot, usually `$n`)
- `<size>` non-negative integer size (non-negative integer, either immediate `#n` or an integer operand)


## Semantics

- Allocates a new list of length `<size>`.
- Initializes every element to `null`.
- Stores the list in `<dst>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $0, #0
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
