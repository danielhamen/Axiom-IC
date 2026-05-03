# `SET_HAS` Instruction

Checks whether a set contains a value.



## Category

- `set`

## Syntax

```
SET_HAS <dst>, <set>, <value>
```

- `<dst>` destination slot for the boolean result
- `<set>` source set
- `<value>` value to search for


## Semantics

- Reads `<set>` as a set
- Searches for `<value>` using VM value equality
- Stores `true` if found, otherwise stores `false`


## Implementation Notes

- This instruction does not mutate the set


## Example

```
.main
start:
  SET_NEW $0
  SET_ADD $0, #1
  SET_HAS $1, $0, #1
  PRINTLN $1
  HALT
```

### Output

```
true
```


## Errors

An error is raised if:

- `<set>` is not a set


## Notes

- Any runtime value can be tested for membership.
