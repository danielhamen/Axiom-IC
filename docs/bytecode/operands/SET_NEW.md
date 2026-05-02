# `SET_NEW` Instruction

Creates a new empty set.


## Syntax

```
SET_NEW <dst>
```

- `<dst>` destination slot where the set is stored (`$n`)


## Semantics

- Allocates an empty set
- Stores the set in `<dst>`
- Set values may be any runtime value


## Implementation Notes

- Internally backed by `std::vector<Value>`
- Uniqueness is checked with the VM's existing value equality rules
- Element order follows insertion order


## Example

```
.main
start:
  SET_NEW $0
  PRINTLN $0
  HALT
```

### Output

```
set{}
```


## Errors

An error is raised if:

- `<dst>` is not a valid writable operand


## Notes

- Use `SET_ADD` to add unique values.
