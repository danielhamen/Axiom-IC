# `LIST_SORT` Instruction

Sorts a list by string representation.



## Category

- `list`

## Syntax

```
LIST_SORT <list>
```

- `<list>` list slot to sort (slot operand, usually `$n`)


## Semantics

- Converts each value with `Value::to_str()` for comparison.
- Sorts the list in ascending lexicographic order.
- Advances to the next instruction unless this instruction explicitly changes control flow.


## Implementation Notes

- Registered in the `list` operation category.
- Lists are backed by `std::vector<Value>`.


## Example

```
.main
start:
  LIST_NEW $1, #0
  LIST_SORT $1
  HALT
```


## Errors

An error is raised if:

- an operand has the wrong kind for this instruction


## Notes

- Operand reads and writes follow the VM operand rules for slots, constants, immediates, and arguments.
