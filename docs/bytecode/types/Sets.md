# Sets

`Set` is a mutable collection of unique runtime values.


## Representation

- Runtime kind: `ValueKind::Set`
- Storage field: `Value::set`
- C++ storage type: `std::vector<Value>`
- Type name returned by `TYPEOF`: `Set`


## Construction

Sets are created with `SET_NEW`:

```aic
SET_NEW $0
```


## Uniqueness

Set uniqueness is enforced with VM value equality.

This means:

- duplicate integers are not added twice
- `#1` and `#1.0` compare equal
- strings compare by exact contents
- null equals null


## String Form

Sets print with `set{...}`:

```text
set{1, 2}
```

Element order follows first insertion order.


## Truthiness

Sets are false only when empty.


## Equality

General VM equality for sets currently falls back to the set string form.

Set membership and uniqueness use VM equality directly.


## Supported Operations

Set operations:

- `SET_NEW`
- `SET_ADD`
- `SET_HAS`
- `SET_DELETE`
- `SET_UNION`
- `SET_INTERSECT`


## Example

```aic
.main
start:
  SET_NEW $0
  SET_ADD $0, #1
  SET_ADD $0, #2
  SET_ADD $0, #2
  PRINTLN $0
  HALT
```

Output:

```text
set{1, 2}
```


## Errors

An error is raised when:

- a set operation receives a non-set where a set is required
- a destination operand is not writable


## Notes

- Sets can contain mixed value types.
- `SET_DELETE` on a missing value is a no-op.
- `SET_UNION` and `SET_INTERSECT` do not mutate their source sets.
