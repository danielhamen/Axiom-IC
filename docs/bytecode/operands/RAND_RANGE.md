# `RAND_RANGE` Instruction

Generates a random floating value in a range.



## Category

- `random`

## Syntax

```
RAND_RANGE <dst>, <min>, <max>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<min>` minimum value (immediate, slot, constant, argument, or supported operand value)
- `<max>` maximum value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Accepts integer or float min and max.
- Stores a random float between them.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `random` operation category.
- Numeric operands accept integers or floats unless the errors section says otherwise.


## Example

```
.main
start:
  RAND_RANGE $0, #0, #1
  HALT
```


## Errors

An error is raised if:

- min or max is not numeric


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
