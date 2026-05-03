# `SET_INTERSECT` Instruction

Creates the intersection of two sets.



## Category

- `set`

## Syntax

```
SET_INTERSECT <dst>, <a>, <b>
```

- `<dst>` destination slot for the result set
- `<a>` first source set
- `<b>` second source set


## Semantics

- Reads `<a>` and `<b>` as sets
- Keeps values from `<a>` that are also present in `<b>`
- Stores the result in `<dst>`


## Implementation Notes

- Result order follows the order of matching values in `<a>`
- Equality uses VM value equality


## Example

```
.main
start:
  SET_NEW $0
  SET_ADD $0, #1
  SET_ADD $0, #2
  SET_NEW $1
  SET_ADD $1, #2
  SET_INTERSECT $2, $0, $1
  PRINTLN $2
  HALT
```

### Output

```
set{2}
```


## Errors

An error is raised if:

- `<a>` is not a set
- `<b>` is not a set
- `<dst>` is not a valid writable operand


## Notes

- The source sets are not mutated.
