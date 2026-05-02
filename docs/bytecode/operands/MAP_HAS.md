# `MAP_HAS` Instruction

Checks whether a map contains a string key.


## Syntax

```
MAP_HAS <dst>, <map>, <key>
```

- `<dst>` destination slot for the boolean result
- `<map>` source map
- `<key>` string key immediate, such as `#~"name"`


## Semantics

- Reads `<map>` as a map
- Reads `<key>` as a string
- Stores `true` when the key exists, otherwise stores `false`


## Implementation Notes

- Uses map key lookup without reading the stored value
- A key mapped to `null` still counts as present


## Example

```
.main
start:
  MAP_NEW $0
  MAP_HAS $1, $0, #~"answer"
  PRINTLN $1
  HALT
```

### Output

```
false
```


## Errors

An error is raised if:

- `<map>` is not a map
- `<key>` is not a string


## Notes

- Use this before `MAP_GET` when a missing key is possible.
