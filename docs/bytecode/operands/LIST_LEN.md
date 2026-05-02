# `LIST_LEN` Instruction

Returns the length of a list.


## Syntax

```
LIST_LEN <dst>, <list>
```

- `<dst>` destination slot for the length (destination slot, usually `$n`)
- `<list>` source list (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Stores the number of elements as an integer.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_LEN $0, $1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
