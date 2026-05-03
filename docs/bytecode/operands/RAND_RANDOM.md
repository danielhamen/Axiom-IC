# `RAND_RANDOM` Instruction

Generates a random decimal.



## Category

- `random`

## Syntax

```
RAND_RANDOM <dst>
```

- `<dst>` destination slot for the random float (destination slot, usually `$n`)


## Semantics

- Stores a float in the range `[0, 1)`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `random` operation category.


## Example

```
.main
start:
  RAND_RANDOM $0
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
