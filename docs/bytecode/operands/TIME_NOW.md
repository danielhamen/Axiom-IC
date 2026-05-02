# `TIME_NOW` Instruction

Returns the current Unix timestamp.


## Syntax

```
TIME_NOW <dst>
```

- `<dst>` destination slot for timestamp (destination slot, usually `$n`)


## Semantics

- Stores seconds since the Unix epoch as a float.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `time` operation category.


## Example

```
.main
start:
  TIME_NOW $0
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
