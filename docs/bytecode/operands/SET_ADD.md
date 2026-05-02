# `SET_ADD` Instruction

Adds a value to a set if it is not already present.


## Syntax

```
SET_ADD <set>, <value>
```

- `<set>` set slot to mutate (`$n`)
- `<value>` value to add; may be any runtime value


## Semantics

- Reads `<set>` as a set
- Adds `<value>` only if an equal value is not already present
- Writes the updated set back to `<set>`


## Implementation Notes

- Equality uses VM value equality
- Numeric integers and floats compare by numeric value
- Adding a duplicate is a no-op


## Example

```
.main
start:
  SET_NEW $0
  SET_ADD $0, #1
  SET_ADD $0, #1
  PRINTLN $0
  HALT
```

### Output

```
set{1}
```


## Errors

An error is raised if:

- `<set>` is not a set


## Notes

- Element order follows first insertion.
