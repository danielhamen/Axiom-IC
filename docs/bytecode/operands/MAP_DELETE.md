# `MAP_DELETE` Instruction

Removes a key from a map.



## Category

- `map`

## Syntax

```
MAP_DELETE <map>, <key>
```

- `<map>` map slot to mutate (`$n`)
- `<key>` string key immediate, such as `#~"name"`


## Semantics

- Reads `<map>` as a map
- Reads `<key>` as a string
- Removes the entry for `<key>` if it exists
- Writes the updated map back to `<map>`


## Implementation Notes

- Deleting a missing key is a no-op
- The instruction does not return the removed value


## Example

```
.main
start:
  MAP_NEW $0
  STORE $1, #42
  MAP_SET $0, #~"answer", $1
  MAP_DELETE $0, #~"answer"
  PRINTLN $0
  HALT
```

### Output

```
{}
```


## Errors

An error is raised if:

- `<map>` is not a map
- `<key>` is not a string


## Notes

- Use `MAP_HAS` if the program needs to know whether the key existed.
