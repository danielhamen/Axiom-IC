# Operands

## ADD

## List operations

### `LIST_NEW $dst, #size`
Create a list with `size` elements initialized to `null` and store it in slot `$dst`.

### `LIST_PUSH $list, <value>`
Append `<value>` to the list in `$list`.

### `LIST_GET $dst, $list, #index`
Read an element from `$list[index]` and place it in `$dst`.

### `LIST_SET $list, #index, <value>`
Update `$list[index]` to `<value>`.

### `LIST_LEN $dst, $list`
Store the current list length in `$dst` as an integer.
