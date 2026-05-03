# `SLEEP` Instruction

Suspends execution for a duration.



## Category

- `time`

## Syntax

```
SLEEP <seconds>
```

- `<seconds>` non-negative numeric duration (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Reads seconds from the operand.
- Sleeps for that duration using `std::this_thread::sleep_for`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `time` operation category.
- Numeric operands accept integers or floats unless the errors section says otherwise.


## Example

```
.main
start:
  SLEEP #0
  HALT
```


## Errors

An error is raised if:

- the duration is negative


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
