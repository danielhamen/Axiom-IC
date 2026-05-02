# `SET_UNION` Instruction

Creates the union of two sets.


## Syntax

```
SET_UNION <dst>, <a>, <b>
```

- `<dst>` destination slot for the result set
- `<a>` first source set
- `<b>` second source set


## Semantics

- Reads `<a>` and `<b>` as sets
- Copies all values from `<a>`
- Adds values from `<b>` that are not already present
- Stores the result in `<dst>`


## Implementation Notes

- The result preserves insertion order from `<a>`, followed by new values from `<b>`
- Equality uses VM value equality


## Example

```
.main
start:
  SET_NEW $0
  SET_ADD $0, #1
  SET_NEW $1
  SET_ADD $1, #2
  SET_UNION $2, $0, $1
  PRINTLN $2
  HALT
```

### Output

```
set{1, 2}
```


## Errors

An error is raised if:

- `<a>` is not a set
- `<b>` is not a set
- `<dst>` is not a valid writable operand


## Notes

- The source sets are not mutated.
