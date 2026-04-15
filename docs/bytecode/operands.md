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

## Numeric lists

### `INT_LIST_NEW $dst, #size`
Create a fixed-size integer list initialized with zeros.

### `INT_LIST_PUSH $list, <int>`
Append an integer to an integer list.

### `INT_LIST_GET $dst, $list, #index`
Read an integer list item into `$dst`.

### `INT_LIST_SET $list, #index, <int>`
Write an integer list item at `index`.

### `INT_LIST_LEN $dst, $list`
Get integer list length.

### `FLOAT_LIST_NEW $dst, #size`
Create a fixed-size float list initialized with `0.0`.

### `FLOAT_LIST_PUSH $list, <number>`
Append a numeric value (int/float) to a float list.

### `FLOAT_LIST_GET $dst, $list, #index`
Read a float list item into `$dst` as float.

### `FLOAT_LIST_SET $list, #index, <number>`
Write a numeric value at a float list index.

### `FLOAT_LIST_LEN $dst, $list`
Get float list length.

## Physics vectors

### `VEC_NEW $dst, #size`
Create a numeric vector initialized with zeros.

### `VEC_PUSH $vec, <number>`
Append a numeric component to vector.

### `VEC_GET $dst, $vec, #index`
Read a component from vector.

### `VEC_SET $vec, #index, <number>`
Write a component in vector.

### `VEC_LEN $dst, $vec`
Get component count.

### `VEC_ADD $dst, $a, $b`
Element-wise vector addition.

### `VEC_SUB $dst, $a, $b`
Element-wise vector subtraction.

### `VEC_SCALE $dst, $a, <number>`
Scale vector by scalar.

### `VEC_DOT $dst, $a, $b`
Dot product.

### `VEC_CROSS $dst, $a, $b`
3D cross product.

### `VEC_MAG $dst, $vec`
Vector magnitude.

### `VEC_NORM $dst, $vec`
Normalized vector.

## Matrices

### `MAT_NEW $dst, #rows, #cols`
Create a zero matrix with row/column shape.

### `MAT_GET $dst, $mat, #index`
Read matrix value using flattened row-major index.

### `MAT_SET $mat, #index, <number>`
Set matrix value using flattened row-major index.

### `MAT_ROWS $dst, $mat`
Get matrix row count.

### `MAT_COLS $dst, $mat`
Get matrix column count.

### `MAT_MUL $dst, $a, $b`
Matrix multiplication.

### `MAT_TRANSPOSE $dst, $mat`
Transpose matrix.

### `MAT_DET $dst, $mat`
Matrix determinant (square matrices only).

### `MAT_INV $dst, $mat`
Matrix inverse (square matrices only, returns `null` for singular matrices).
