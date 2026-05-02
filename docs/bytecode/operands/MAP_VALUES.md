# `MAP_VALUES` Instruction

Returns the values from a map as a list.


## Syntax

```
MAP_VALUES <dst>, <map>
```

- `<dst>` destination slot for the list
- `<map>` source map


## Semantics

- Reads `<map>` as a map
- Creates a list containing each stored value
- Stores the list in `<dst>`


## Implementation Notes

- Values may be any runtime value
- Value order matches the map key order used by `MAP_KEYS`


## Example

```
.main
start:
  MAP_NEW $0
  STORE $1, #42
  MAP_SET $0, #~"answer", $1
  MAP_VALUES $2, $0
  PRINTLN $2
  HALT
```

### Output

```
[42]
```


## Errors

An error is raised if:

- `<map>` is not a map
- `<dst>` is not a valid writable operand


## Notes

- The returned list is independent of the map.
