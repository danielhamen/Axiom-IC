# `RAND_INT` Instruction

Generates a random integer in an inclusive range.



## Category

- `random`

## Syntax

```
RAND_INT <dst>, <min>, <max>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<min>` minimum value (immediate, slot, constant, argument, or supported operand value)
- `<max>` maximum value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Requires integer min and max.
- Stores a random integer between them.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `random` operation category.


## Example

```
.main
start:
  RAND_INT $0, #0, #1
  HALT
```


## Errors

An error is raised if:

- min or max is not an integer


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
