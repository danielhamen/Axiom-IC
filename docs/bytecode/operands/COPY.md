# `COPY` Instruction

Copies memory from one slot to another

## Category

- `memory`

## Syntax

```
COPY <dst>, <src>
```

- `dst` target slot
- `src` source slot

## Semantics

- Writes directly to `dst` from `src` using `write_operand`
- Advances to next instruction



## Implementation Notes

- Registered in the `memory` operation category.

## Example

```
.main
start:
  STORE   $0, #1
  COPY    $1, $0
  PRINTLN $1 ; output "1"
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction
