# `MAP_GET` Instruction

Reads a value from a map by string key.



## Category

- `map`

## Syntax

```
MAP_GET <dst>, <map>, <key>
```

- `<dst>` destination slot for the value
- `<map>` source map
- `<key>` string key immediate, such as `#~"name"`


## Semantics

- Reads `<map>` as a map
- Reads `<key>` as a string
- Stores the value for `<key>` in `<dst>`


## Implementation Notes

- Missing keys are errors rather than returning `null`
- Use `MAP_HAS` to check whether a key exists before reading it


## Example

```
.main
start:
  MAP_NEW $0
  STORE $1, #42
  MAP_SET $0, #~"answer", $1
  MAP_GET $2, $0, #~"answer"
  PRINTLN $2
  HALT
```

### Output

```
42
```


## Errors

An error is raised if:

- `<map>` is not a map
- `<key>` is not a string
- `<key>` does not exist in the map


## Notes

- This instruction does not mutate the map.
