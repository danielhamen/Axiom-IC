# `LIST_FILL` Instruction

Appends the same value to a list multiple times.

## Category

- `list`

## Syntax

```
LIST_FILL <list>, <count>, <value>
```

- `<list>` list slot or writable literal alias to mutate
- `<count>` non-negative integer number of entries to append
- `<value>` value copied into each appended entry

## Semantics

- Reads `<list>` as a list.
- Reads `<count>` once.
- Reads `<value>` once.
- Appends `<count>` copies of `<value>` to the end of the list.
- Advances to the next instruction.

## Optimizer

The peephole optimizer folds repeated identical immediate or constant pushes into `LIST_FILL`.

```
LIST_NEW $0, #0
LIST_PUSH $0, #0
LIST_PUSH $0, #0
LIST_PUSH $0, #0
```

becomes:

```
LIST_NEW $0, #0
LIST_FILL $0, #3, #0
```

## Example

```
.import <list>
.import <control_flow>

.main
  LIST_NEW $0, #0
  LIST_FILL $0, #4, #null
  HALT
```

## Errors

An error is raised if:

- `<list>` is not a writable list
- `<count>` is not a non-negative integer
