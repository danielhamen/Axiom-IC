# `PRINTLN` Instruction

Writes a value to standard output followed by a newline.


## Syntax

```
PRINTLN <value>
```

- `<value>` value to print (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Converts the operand with `Value::to_str()`.
- Appends `std::endl`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `io` operation category.


## Example

```
.main
start:
  PRINTLN #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
