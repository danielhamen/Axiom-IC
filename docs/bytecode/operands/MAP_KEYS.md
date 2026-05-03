# `MAP_KEYS` Instruction

Returns the keys from a map as a list of strings.



## Category

- `map`

## Syntax

```
MAP_KEYS <dst>, <map>
```

- `<dst>` destination slot for the list
- `<map>` source map


## Semantics

- Reads `<map>` as a map
- Creates a list containing each key as a string value
- Stores the list in `<dst>`


## Implementation Notes

- Map storage is ordered by string key
- The returned key list follows that lexicographic order


## Example

```
.main
start:
  MAP_NEW $0
  STORE $1, #42
  MAP_SET $0, #~"answer", $1
  MAP_KEYS $2, $0
  PRINTLN $2
  HALT
```

### Output

```
[answer]
```


## Errors

An error is raised if:

- `<map>` is not a map
- `<dst>` is not a valid writable operand


## Notes

- The returned list is independent of the map.
