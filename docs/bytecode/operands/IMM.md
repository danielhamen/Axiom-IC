# `IMM` Instruction

Marks a local slot as immutable.



## Category

- `memory`

## Syntax

```
IMM <slot>
```

- `<slot>` slot to make immutable, such as `$0`


## Semantics

- Marks `<slot>` immutable in the active local frame.
- Later assignments to the slot fail. This includes `STORE`, `LOAD`, `INC`, `DEC`, `SWAP`, `CLEAR`, and other instructions that write a new value to that slot.
- The immutable mark belongs to the current frame. The `.main` root frame and each called function frame track immutable slots separately.
- Immutability freezes the slot binding, not the internals of the value. For example, if `$0` contains a struct, `STRUCT_SET $0, #~"field", <value>` can still update a mutable public field, but `STORE $0, <value>` cannot replace the whole struct.


## Implementation Notes

- Registered in the `memory` operation category.


## Example

```
.import <memory>
.import <arithmetic>
.import <control_flow>

.main
  STORE $0, #1
  IMM $0
  INC $0, $0
  HALT
```

The `INC $0, $0` instruction raises an error because `$0` is immutable.


## Errors

An error is raised if:

- `<slot>` is not a slot operand
- a later instruction tries to reassign the immutable slot


## Notes

- `IMM` is one-way for the lifetime of the active frame; there is no operation to make the slot mutable again.
