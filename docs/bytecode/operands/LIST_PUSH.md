# `LIST_PUSH` Instruction

Appends a value to a list.


## Syntax

```
LIST_PUSH <list>, <value>
```

- `<list>` list slot to mutate (slot operand, usually `$n`)
- `<value>` value to append (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads `<list>` as a list.
- Appends `<value>` to the end.
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
  LIST_PUSH $1, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
