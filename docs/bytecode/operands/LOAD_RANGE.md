# `LOAD_RANGE` Instruction

Loads one value into an inclusive range of slots.

## Category

- `memory`

## Syntax

```
LOAD_RANGE <start_slot_idx>, <end_slot_idx>, <value>
```

- `<start_slot_idx>` non-negative integer index of the first slot to write
- `<end_slot_idx>` non-negative integer index of the last slot to write
- `<value>` value copied into each destination slot

## Semantics

- Reads `<start_slot_idx>` and `<end_slot_idx>`.
- Reads `<value>` once before any slot is written.
- Writes the copied value into every slot from `<start_slot_idx>` through `<end_slot_idx>`, inclusive.
- Advances to the next instruction.

## Optimizer

The peephole optimizer folds consecutive same-value slot loads into `LOAD_RANGE`.

```
LOAD $1, #0
LOAD $2, #0
LOAD $3, #0
```

becomes:

```
LOAD_RANGE #1, #3, #0
```

## Example

```
.import <memory>
.import <control_flow>

.main
  LOAD_RANGE #1, #25, #0
  HALT
```

## Errors

An error is raised if:

- either slot index is not a non-negative integer
- `<end_slot_idx>` is smaller than `<start_slot_idx>`
