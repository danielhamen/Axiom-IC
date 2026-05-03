# `MAP_NEW` Instruction

Creates a new empty map.



## Category

- `map`

## Syntax

```
MAP_NEW <dst>
```

- `<dst>` destination slot where the map is stored (`$n`)


## Semantics

- Allocates an empty key-to-value dictionary
- Keys are strings
- Values may be any runtime value
- Stores the map in `<dst>`


## Implementation Notes

- Internally backed by `std::map<std::string, Value>`
- Map keys are ordered lexicographically
- The map is mutable through `MAP_SET` and `MAP_DELETE`


## Example

```
.main
start:
  MAP_NEW $0
  PRINTLN $0
  HALT
```

### Output

```
{}
```


## Errors

An error is raised if:

- `<dst>` is not a valid writable operand


## Notes

- This creates an empty map; use `MAP_SET` to add entries.
