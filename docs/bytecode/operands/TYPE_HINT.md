# `TYPE_HINT` Instruction

Assigns a runtime type hint to a slot and enforces future writes to that slot.


## Category

- `type`

## Syntax

```
TYPE_HINT <slot>, <type>
```

- `<slot>` slot to annotate (`$n`)
- `<type>` string type expression, such as `#~"Integer|Float"`


## Semantics

- Reads `<type>` as a string.
- Validates the type expression.
- Stores the type hint for `<slot>` in the active slot frame.
- Future writes to that slot must match the type expression.


## Implementation Notes

- Slot hints are local to the active call frame.
- `.main` stores hints in root program slot hints.
- Called functions store hints in `CallFrame::slot_type_hints`.
- Hints are enforced by `write_operand`, including `STORE`.


## Example

```
.import <type>
.import <memory>
.import <control_flow>

.main
start:
  TYPE_HINT $0, #~"Integer|Float"
  STORE $0, #1
  STORE $0, #2.5
  HALT
```


## Errors

An error is raised if:

- `<slot>` is not a slot
- `<type>` is not a string
- `<type>` is an invalid type expression
- a later write to `<slot>` does not match the hint


## Notes

- `TYPE_HINT` does not validate any existing value already in the slot. It constrains subsequent writes.
