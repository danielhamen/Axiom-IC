# `NOT` Instruction

Performs boolean NOT or integer bitwise complement.


## Syntax

```
NOT <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Boolean operands store `!value`.
- Integer operands store bitwise `~value`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `logic` operation category.


## Example

```
.main
start:
  NOT $0, #1
  HALT
```


## Errors

An error is raised if:

- the operand is neither boolean nor integer


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
