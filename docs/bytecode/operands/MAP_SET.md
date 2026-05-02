# `MAP_SET` Instruction

Sets a string key to a value in a map.


## Syntax

```
MAP_SET <map>, <key>, <value>
```

- `<map>` map slot to mutate (`$n`)
- `<key>` string key immediate, such as `#~"name"`
- `<value>` value to store; may be any runtime value


## Semantics

- Reads `<map>` as a map
- Reads `<key>` as a string
- Stores `<value>` under that key
- Replaces the previous value if the key already exists
- Writes the updated map back to `<map>`


## Implementation Notes

- Keys are stored as `std::string`
- Values are stored as `Value`, so nested lists, maps, vectors, matrices, strings, numbers, booleans, and null are allowed


## Example

```
.main
start:
  MAP_NEW $0
  STORE $1, #42
  MAP_SET $0, #~"answer", $1
  PRINTLN $0
  HALT
```

### Output

```
{answer: 42}
```


## Errors

An error is raised if:

- `<map>` is not a map
- `<key>` is not a string


## Notes

- Bare quoted strings are not valid operands; use `#~"..."` for string immediates.
