# `ACOSH` Instruction

Returns the inverse hyperbolic cosine.


## Syntax

```
ACOSH <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Computes `acosh(value)`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `math` operation category.
- Numeric operands accept integers or floats unless the errors section says otherwise.


## Example

```
.main
start:
  ACOSH $0, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
