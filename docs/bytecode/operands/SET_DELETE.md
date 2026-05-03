# `SET_DELETE` Instruction

Removes a value from a set.



## Category

- `set`

## Syntax

```
SET_DELETE <set>, <value>
```

- `<set>` set slot to mutate (`$n`)
- `<value>` value to remove


## Semantics

- Reads `<set>` as a set
- Removes the first equal value if present
- Writes the updated set back to `<set>`


## Implementation Notes

- Deleting a missing value is a no-op
- Equality uses VM value equality


## Example

```
.main
start:
  SET_NEW $0
  SET_ADD $0, #1
  SET_DELETE $0, #1
  PRINTLN $0
  HALT
```

### Output

```
set{}
```


## Errors

An error is raised if:

- `<set>` is not a set


## Notes

- Use `SET_HAS` if the program needs to know whether the value existed.
