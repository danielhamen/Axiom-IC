# `LIST_INSERT` Instruction

Inserts a value into a list at an index.


## Syntax

```
LIST_INSERT <list>, <index>, <value>
```

- `<list>` list slot to mutate (slot operand, usually `$n`)
- `<index>` non-negative insertion index (non-negative integer, either immediate `#n` or an integer operand)
- `<value>` value to insert (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Inserts `<value>` before `<index>`.
- Allows `<index>` equal to the current list length to append.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_INSERT $1, #0, #1
  HALT
```


## Errors

An error is raised if:

- an index is negative or out of bounds


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
