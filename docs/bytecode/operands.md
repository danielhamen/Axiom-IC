# Operands

## ADD

## Vector operations

### `VNEW $dst, #size`
Create a vector with `size` elements initialized to `null` and store it in slot `$dst`.

### `VPUSH $vec, <value>`
Append `<value>` to the vector in `$vec`.

### `VGET $dst, $vec, #index`
Read an element from `$vec[index]` and place it in `$dst`.

### `VSET $vec, #index, <value>`
Update `$vec[index]` to `<value>`.

### `VLEN $dst, $vec`
Store the current vector length in `$dst` as an integer.
