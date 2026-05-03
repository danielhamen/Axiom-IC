# `LIST_SET` Instruction

Writes a list element by index.



## Category

- `list`

## Syntax

```
LIST_SET <list>, <index>, <value>
```

- `<list>` list slot to mutate (slot operand, usually `$n`)
- `<index>` non-negative element index (non-negative integer, either immediate `#n` or an integer operand)
- `<value>` replacement value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Replaces `<list>[<index>]` with `<value>`.
- Writes the updated list back to `<list>`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_SET $1, #0, #1
  HALT
```


## Errors

An error is raised if:

- an index is negative or out of bounds


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
