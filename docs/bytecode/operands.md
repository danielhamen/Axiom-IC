# Bytecode operands

Operand prefixes:
- `$n` slot operand.
- `#value` immediate operand.
- `@n` constant-table index.
- `&n` absolute memory address (for `LOAD`/`STORE`).
- `label` label operand.

## List operations
- `LIST_NEW $dst, #size`
- `LIST_PUSH $list, <value>`
- `LIST_POP $dst, $list`
- `LIST_INSERT $list, #index, <value>`
- `LIST_ERASE $list, #index`
- `LIST_SLICE $dst, $list, #start` (slice from `start` to end)
- `LIST_CLEAR $list`
- `LIST_FIND $dst, $list, <value>` (returns index or `-1`)
- `LIST_SORT $list`
- `LIST_REVERSE $list`
- `LIST_GET $dst, $list, #index`
- `LIST_SET $list, #index, <value>`
- `LIST_LEN $dst, $list`

## Numeric list operations
- `INT_LIST_NEW $dst, #size`
- `INT_LIST_PUSH $list, <int>`
- `INT_LIST_GET $dst, $list, #index`
- `INT_LIST_SET $list, #index, <int>`
- `INT_LIST_LEN $dst, $list`
- `FLOAT_LIST_NEW $dst, #size`
- `FLOAT_LIST_PUSH $list, <number>`
- `FLOAT_LIST_GET $dst, $list, #index`
- `FLOAT_LIST_SET $list, #index, <number>`
- `FLOAT_LIST_LEN $dst, $list`

## Vector operations
- `VEC_NEW $dst, #size`
- `VEC_PUSH $vec, <number>`
- `VEC_GET $dst, $vec, #index`
- `VEC_SET $vec, #index, <number>`
- `VEC_LEN $dst, $vec`
- `VEC_ADD $dst, $a, $b`
- `VEC_SUB $dst, $a, $b`
- `VEC_SCALE $dst, $vec, <number>`
- `VEC_DOT $dst, $a, $b`
- `VEC_CROSS $dst, $a, $b`
- `VEC_MAG $dst, $vec`
- `VEC_NORM $dst, $vec`
- `VEC_PROJECT $dst, $a, $b`
- `VEC_REFLECT $dst, $v, $normal`
- `VEC_ANGLE $dst, $a, $b` (radians)
- `VEC_DISTANCE $dst, $a, $b`
- `VEC_LERP $dst, $a, $b` (`$dst` is read as `t` in `[0,1]`, then overwritten)

## Matrix operations
- `MAT_NEW $dst, #rows, #cols`
- `MAT_GET $dst, $mat, #flat_index`
- `MAT_SET $mat, #flat_index, <number>`
- `MAT_ROWS $dst, $mat`
- `MAT_COLS $dst, $mat`
- `MAT_MUL $dst, $a, $b`
- `MAT_ADD $dst, $a, $b`
- `MAT_SUB $dst, $a, $b`
- `MAT_SCALE $dst, $mat, <number>`
- `MAT_IDENTITY $dst, #size`
- `MAT_TRACE $dst, $mat`
- `MAT_RREF $dst, $mat`
- `MAT_RANK $dst, $mat`
- `MAT_EIGEN $dst, $mat` (2x2 real eigenvalues)
- `MAT_SOLVE $dst, $A, $b` (`$b` is a vector)
- `MAT_TRANSPOSE $dst, $mat`
- `MAT_DET $dst, $mat`
- `MAT_INV $dst, $mat`

## String operations
- `STR_CONCAT $dst, $a, $b`
- `STR_LEN $dst, $str`
- `STR_SUBSTR $dst, $str, #start`
- `STR_FIND $dst, $haystack, $needle`
- `STR_REPLACE $str, $from, $to` (in-place)
- `STR_SPLIT $dst, $str, $delimiter`
- `STR_JOIN $dst, $list_of_strings, $separator`
- `STR_UPPER $dst, $str`
- `STR_LOWER $dst, $str`
- `STR_TRIM $dst, $str`
- `STR_EQ $dst, $a, $b`

## Type/cast operations
- `TYPEOF $dst, <value>`
- `CAST_INT $dst, <value>`
- `CAST_FLOAT $dst, <value>`
- `CAST_BOOL $dst, <value>`
- `CAST_STRING $dst, <value>`
- `IS_NULL $dst, <value>`
- `IS_NAN $dst, <value>`
- `IS_INF $dst, <value>`

## File/time operations
- `READ_FILE $dst, $path`
- `WRITE_FILE $path, $content`
- `APPEND_FILE $path, $content`
- `FILE_EXISTS $dst, $path`
- `DELETE_FILE $path`
- `TIME_NOW $dst`
- `SLEEP <seconds>`

## Jump operations
- `JMP label`
- `JMP_IF label, <cond>`
- `JMP_IF_ZERO label, <number>`
- `JMP_IF_NOT label, <cond>`
- `JMP_EQ label, <a>, <b>`
- `JMP_NEQ label, <a>, <b>`
- `JMP_LT label, <a>, <b>`
- `JMP_LTE label, <a>, <b>`
- `JMP_GT label, <a>, <b>`
- `JMP_GTE label, <a>, <b>`

## Memory/slot operations
- `LOAD $dst, &addr`
- `STORE &addr, <value>`
- `MOVE $dst, <value>`
- `SWAP $a, $b`
- `DUP $dst, <value>`
- `CLEAR $slot`

## Extra math operations
- `SIGN $dst, <number>`
- `CBRT $dst, <number>`
- `FACTORIAL $dst, <int>`
- `GCD $dst, <int>, <int>`
- `LCM $dst, <int>, <int>`
- `REM $dst, <number>, <number>`
- `FMOD $dst, <number>, <number>`
- `DEG2RAD $dst, <number>`
- `RAD2DEG $dst, <number>`
- `LERP $dst, <a>, <b>` (`$dst` is read as `t` in `[0,1]`, then overwritten)
- `MAP_RANGE $dst, <in_min>, <in_max>` (`$dst` is read as the input value, then overwritten with normalized output)
