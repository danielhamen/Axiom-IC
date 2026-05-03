# `FACTORIAL` Instruction

Computes an integer factorial.



## Category

- `math`

## Syntax

```
FACTORIAL <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Requires a non-negative integer.
- Stores the result as an integer.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `math` operation category.


## Example

```
.main
start:
  FACTORIAL $0, #1
  HALT
```


## Errors

An error is raised if:

- the value is not a non-negative integer


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
