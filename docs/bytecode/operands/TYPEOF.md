# `TYPEOF` Instruction

Returns the runtime value kind.



## Category

- `type`

## Syntax

```
TYPEOF <dst>, <value>
```

- `<dst>` destination slot (destination slot, usually `$n`)
- `<value>` source value (immediate, slot, constant, argument, or supported operand value)


## Semantics

- Stores one of the VM type names such as `Integer`, `Float`, `String`, `Boolean`, `Null`, `List`, `Vector`, or `Matrix`.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `type` operation category.


## Example

```
.main
start:
  TYPEOF $0, #1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
