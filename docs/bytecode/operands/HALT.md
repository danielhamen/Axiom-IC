# `HALT` Instruction

Stops program execution.


## Syntax

```
HALT
```

- Takes no operands.


## Semantics

- Sets the VM halted flag.
- Does not advance the program counter.


## Implementation Notes

- Registered in the `control_flow` operation category.


## Example

```
.main
start:
  HALT
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- No later instruction executes after `HALT` unless execution is externally restarted.
