# `LIST_NEW` Instruction

Creates a new list with a fixed initial size.


## Syntax

```
LIST_NEW <dst>, <size>
```

- `<dst>` destination **slot** where the list is stored (`$n`)
- `<size>` non-negative integer (immediate integer `#n` or passed integer)


## Semantics

- Allocates a new list of length `<size>`
- All elements are initialized to `null`
- The resulting list is stored in `<dst>`


## Implementation Notes

- Internally backed by `std::vector<Value>`
- Each element is initialized to `Value::Null`
- The list is mutable and dynamically resizable via other list operations


## Example

```
.main
start:
  LIST_NEW $0, #4
  PRINTLN $0
  HALT
```

### Output

```
[null, null, null, null]
```


## Errors

An error is raised if:

- `<size>` is not an integer
- `<size>` is negative
- `<dst>` is not a valid slot


## Notes

- This creates a **pre-sized list**, not an empty list  
  (use size `#0` for an empty list)
- Subsequent operations like `LIST_PUSH` may increase the size
