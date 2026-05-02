# Maps

`Map` is a mutable string-keyed dictionary.


## Representation

- Runtime kind: `ValueKind::Map`
- Storage field: `Value::map`
- C++ storage type: `std::map<std::string, Value>`
- Type name returned by `TYPEOF`: `Map`


## Construction

Maps are created with `MAP_NEW`:

```aic
MAP_NEW $0
```


## Keys and Values

Map keys must be strings.

The correct string immediate syntax is:

```aic
#~"answer"
```

Bare quoted strings such as `"answer"` are invalid operands.

Values may be any runtime value.


## String Form

Maps print with braces and `key: value` pairs:

```text
{answer: 42}
```

Keys are printed without quotes. Values use their own `to_str()` representation.


## Truthiness

Maps are false only when empty.


## Equality

General VM equality for maps currently falls back to the map string form.

Because maps use `std::map`, key order is lexicographic and stable.


## Supported Operations

Map operations:

- `MAP_NEW`
- `MAP_SET`
- `MAP_GET`
- `MAP_HAS`
- `MAP_DELETE`
- `MAP_KEYS`
- `MAP_VALUES`


## Example

```aic
.main
start:
  MAP_NEW $0
  STORE $1, #42
  MAP_SET $0, #~"answer", $1
  MAP_GET $2, $0, #~"answer"
  PRINTLN $2
  HALT
```

Output:

```text
42
```


## Errors

An error is raised when:

- a map operation receives a non-map where a map is required
- a key operand is not a string
- `MAP_GET` is used for a missing key


## Notes

- `MAP_DELETE` on a missing key is a no-op.
- `MAP_KEYS` returns a list of string keys.
- `MAP_VALUES` returns a list of values ordered by key order.
