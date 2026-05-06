# Lists

`List` is an ordered, mutable sequence of runtime values.


## Representation

- Runtime kind: `ValueKind::List`
- Storage field: `Value::list`
- C++ storage type: `std::vector<Value>`
- Type name returned by `TYPEOF`: `List`


## Construction

Lists are created with `LIST_NEW`:

```aic
LIST_NEW $0, #3
```

The size operand is a non-negative integer.


## Initialization

`LIST_NEW` creates a pre-sized list and initializes every element to null.

```aic
.main
start:
  LIST_NEW $0, #3
  PRINTLN $0
  HALT
```

Output:

```text
[null, null, null]
```


## String Form

Lists print with square brackets. Each element uses its own `to_str()` representation.

```text
[1, hello, null]
```


## Truthiness

Lists are false only when empty.

- `LIST_NEW $0, #0` creates a falsey list
- any non-empty list is truthy, even if all elements are null


## Equality

General VM equality for lists currently falls back to the list string form.

`LIST_FIND`, `SET_HAS`, and set uniqueness use VM equality.


## Supported Operations

List operations:

- `LIST_NEW`
- `LIST_PUSH`
- `LIST_FILL`
- `LIST_POP`
- `LIST_INSERT`
- `LIST_ERASE`
- `LIST_SLICE`
- `LIST_CLEAR`
- `LIST_FIND`
- `LIST_SORT`
- `LIST_REVERSE`
- `LIST_GET`
- `LIST_SET`
- `LIST_LEN`

String splitting returns a list:

- `STR_SPLIT`

Map extraction returns lists:

- `MAP_KEYS`
- `MAP_VALUES`


## Mutability

List mutation operations read the list value, modify it, and write it back to the list slot.

Examples:

```aic
LIST_PUSH $0, #1
LIST_FILL $0, #8, #0
LIST_SET $0, #0, #2
LIST_ERASE $0, #0
```


## Errors

An error is raised when:

- a list operation receives a non-list where a list is required
- an index is negative or out of bounds
- `LIST_POP` is used on an empty list


## Notes

- Lists can contain values of mixed types.
- Lists can contain nested lists, maps, sets, vectors, and matrices.
