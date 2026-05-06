# Bytecode operands

Operand prefixes:
- `$n` slot operand.
- `#value` immediate operand.
- `@n` constant-table index.
- `label` label operand.

Slot operands are local to the active function call frame. The `.main` entry point has its own root slot frame, and each `CALL` creates a separate slot frame for the callee.

Directives:
- `.import <category>` imports an operation category. Import directives must appear at the top of the file before `.const`, `.main`, `.fn`, labels, constants, or instructions.
- `.fn <label>` defines a function named `<label>`.
- `.main` marks the main entry point.
- `.const` defines constants. It can open a constant section or declare one anonymous inline constant, such as `.const FLOAT 3.14159`. Inline and section constants can also specify an explicit constant-table index, such as `.const 55 FLOAT 5.5`; missing indexes are filled with `null`.

Every instruction requires its category to be imported. For example, `ADD` requires `.import <arithmetic>`, `LIST_NEW` requires `.import <list>`, and `HALT` requires `.import <control_flow>`.

Immediate values:
- booleans (`#true` or `#false`)
- numbers (`#3.5` or `#-3`)
- strings (`#~"She said \"Hello World\""`)
- null (`#null`)

Each operation below has a full page in `docs/bytecode/operands/<OPCODE>.md`.

## Arithmetic, Comparison, Logic, and Math Operations

- [`ADD`](operands/ADD.md): `ADD <dst>, <lhs>, <rhs>`
- [`SUB`](operands/SUB.md): `SUB <dst>, <lhs>, <rhs>`
- [`MUL`](operands/MUL.md): `MUL <dst>, <lhs>, <rhs>`
- [`DIV`](operands/DIV.md): `DIV <dst>, <lhs>, <rhs>`
- [`MOD`](operands/MOD.md): `MOD <dst>, <lhs>, <rhs>`
- [`NEG`](operands/NEG.md): `NEG <dst>, <value>`
- [`INC`](operands/INC.md): `INC <dst>, <value>`
- [`DEC`](operands/DEC.md): `DEC <dst>, <value>`
- [`ABS`](operands/ABS.md): `ABS <dst>, <value>`

## Comparison

- [`EQ`](operands/EQ.md): `EQ <dst>, <lhs>, <rhs>`
- [`NEQ`](operands/NEQ.md): `NEQ <dst>, <lhs>, <rhs>`
- [`LT`](operands/LT.md): `LT <dst>, <lhs>, <rhs>`
- [`LTE`](operands/LTE.md): `LTE <dst>, <lhs>, <rhs>`
- [`GT`](operands/GT.md): `GT <dst>, <lhs>, <rhs>`
- [`GTE`](operands/GTE.md): `GTE <dst>, <lhs>, <rhs>`

## Logic

- [`AND`](operands/AND.md): `AND <dst>, <lhs>, <rhs>`
- [`OR`](operands/OR.md): `OR <dst>, <lhs>, <rhs>`
- [`NOT`](operands/NOT.md): `NOT <dst>, <value>`
- [`XOR`](operands/XOR.md): `XOR <dst>, <lhs>, <rhs>`

## Math Operations

- [`POW`](operands/POW.md): `POW <dst>, <lhs>, <rhs>`
- [`SQRT`](operands/SQRT.md): `SQRT <dst>, <value>`
- [`EXP`](operands/EXP.md): `EXP <dst>, <value>`
- [`LOG`](operands/LOG.md): `LOG <dst>, <lhs>, <rhs>`
- [`LOG2`](operands/LOG2.md): `LOG2 <dst>, <value>`
- [`LOG10`](operands/LOG10.md): `LOG10 <dst>, <value>`
- [`LN`](operands/LN.md): `LN <dst>, <value>`
- [`SIN`](operands/SIN.md): `SIN <dst>, <value>`
- [`COS`](operands/COS.md): `COS <dst>, <value>`
- [`TAN`](operands/TAN.md): `TAN <dst>, <value>`
- [`CSC`](operands/CSC.md): `CSC <dst>, <value>`
- [`SEC`](operands/SEC.md): `SEC <dst>, <value>`
- [`COT`](operands/COT.md): `COT <dst>, <value>`
- [`ASIN`](operands/ASIN.md): `ASIN <dst>, <value>`
- [`ACOS`](operands/ACOS.md): `ACOS <dst>, <value>`
- [`ATAN`](operands/ATAN.md): `ATAN <dst>, <value>`
- [`ACSC`](operands/ACSC.md): `ACSC <dst>, <value>`
- [`ASEC`](operands/ASEC.md): `ASEC <dst>, <value>`
- [`ACOT`](operands/ACOT.md): `ACOT <dst>, <value>`
- [`SINH`](operands/SINH.md): `SINH <dst>, <value>`
- [`COSH`](operands/COSH.md): `COSH <dst>, <value>`
- [`TANH`](operands/TANH.md): `TANH <dst>, <value>`
- [`CSCH`](operands/CSCH.md): `CSCH <dst>, <value>`
- [`SECH`](operands/SECH.md): `SECH <dst>, <value>`
- [`COTH`](operands/COTH.md): `COTH <dst>, <value>`
- [`ASINH`](operands/ASINH.md): `ASINH <dst>, <value>`
- [`ACOSH`](operands/ACOSH.md): `ACOSH <dst>, <value>`
- [`ATANH`](operands/ATANH.md): `ATANH <dst>, <value>`
- [`ACSCH`](operands/ACSCH.md): `ACSCH <dst>, <value>`
- [`ASECH`](operands/ASECH.md): `ASECH <dst>, <value>`
- [`ACOTH`](operands/ACOTH.md): `ACOTH <dst>, <value>`
- [`ATAN2`](operands/ATAN2.md): `ATAN2 <dst>, <lhs>, <rhs>`
- [`HYPOT`](operands/HYPOT.md): `HYPOT <dst>, <lhs>, <rhs>`
- [`FLOOR`](operands/FLOOR.md): `FLOOR <dst>, <value>`
- [`CEIL`](operands/CEIL.md): `CEIL <dst>, <value>`
- [`ROUND`](operands/ROUND.md): `ROUND <dst>, <value>`
- [`MIN`](operands/MIN.md): `MIN <dst>, <lhs>, <rhs>`
- [`MAX`](operands/MAX.md): `MAX <dst>, <lhs>, <rhs>`
- [`CLAMP`](operands/CLAMP.md): `CLAMP <dst>, <min>, <max>`
- [`SIGN`](operands/SIGN.md): `SIGN <dst>, <value>`
- [`CBRT`](operands/CBRT.md): `CBRT <dst>, <value>`
- [`FACTORIAL`](operands/FACTORIAL.md): `FACTORIAL <dst>, <value>`
- [`GCD`](operands/GCD.md): `GCD <dst>, <lhs>, <rhs>`
- [`LCM`](operands/LCM.md): `LCM <dst>, <lhs>, <rhs>`
- [`REM`](operands/REM.md): `REM <dst>, <lhs>, <rhs>`
- [`FMOD`](operands/FMOD.md): `FMOD <dst>, <lhs>, <rhs>`
- [`DEG2RAD`](operands/DEG2RAD.md): `DEG2RAD <dst>, <value>`
- [`RAD2DEG`](operands/RAD2DEG.md): `RAD2DEG <dst>, <value>`
- [`LERP`](operands/LERP.md): `LERP <dst>, <a>, <b>`
- [`MAP_RANGE`](operands/MAP_RANGE.md): `MAP_RANGE <dst>, <in_min>, <in_max>`

## Control Flow Operations

- [`JMP`](operands/JMP.md): `JMP <label>`
- [`HALT`](operands/HALT.md): `HALT`
- [`JMP_IF`](operands/JMP_IF.md): `JMP_IF <label>, <condition>`
- [`JMP_IF_ZERO`](operands/JMP_IF_ZERO.md): `JMP_IF_ZERO <label>, <condition>`
- [`JMP_IF_NOT`](operands/JMP_IF_NOT.md): `JMP_IF_NOT <label>, <condition>`
- [`JMP_EQ`](operands/JMP_EQ.md): `JMP_EQ <label>, <lhs>, <rhs>`
- [`JMP_NEQ`](operands/JMP_NEQ.md): `JMP_NEQ <label>, <lhs>, <rhs>`
- [`JMP_LT`](operands/JMP_LT.md): `JMP_LT <label>, <lhs>, <rhs>`
- [`JMP_LTE`](operands/JMP_LTE.md): `JMP_LTE <label>, <lhs>, <rhs>`
- [`JMP_GT`](operands/JMP_GT.md): `JMP_GT <label>, <lhs>, <rhs>`
- [`JMP_GTE`](operands/JMP_GTE.md): `JMP_GTE <label>, <lhs>, <rhs>`

## Exception Operations

- [`TRY`](operands/TRY.md): `TRY <catch_label>`
- [`CATCH`](operands/CATCH.md): `CATCH`
- [`FINALLY`](operands/FINALLY.md): `FINALLY <label>`
- [`END_TRY`](operands/END_TRY.md): `END_TRY`
- [`THROW`](operands/THROW.md): `THROW <error>`
- [`ERR_GET`](operands/ERR_GET.md): `ERR_GET <dst>`
- [`ERR_CLEAR`](operands/ERR_CLEAR.md): `ERR_CLEAR`
- [`ERROR_NEW`](operands/ERROR_NEW.md): `ERROR_NEW <dst>, <type>, <message>`
- [`ERROR_TYPE`](operands/ERROR_TYPE.md): `ERROR_TYPE <dst>, <error>`
- [`ERROR_MESSAGE`](operands/ERROR_MESSAGE.md): `ERROR_MESSAGE <dst>, <error>`
- [`ERROR_IS`](operands/ERROR_IS.md): `ERROR_IS <dst>, <error>, <type>`

## Literal Operations

- [`LITERAL`](operands/LITERAL.md): `LITERAL <name>, <target>`

## I/O Operations

- [`PRINT`](operands/PRINT.md): `PRINT <value>`
- [`PRINTLN`](operands/PRINTLN.md): `PRINTLN <value>`
- [`READ_FILE`](operands/READ_FILE.md): `READ_FILE <dst>, <value>`
- [`WRITE_FILE`](operands/WRITE_FILE.md): `WRITE_FILE <path>, <content>`
- [`APPEND_FILE`](operands/APPEND_FILE.md): `APPEND_FILE <path>, <content>`
- [`FILE_EXISTS`](operands/FILE_EXISTS.md): `FILE_EXISTS <dst>, <value>`
- [`DELETE_FILE`](operands/DELETE_FILE.md): `DELETE_FILE <path>`

## Stack Operations

- [`PUSH`](operands/PUSH.md): `PUSH <value>`
- [`POP`](operands/POP.md): `POP <dst>`

## Function Operations

- [`CALL`](operands/CALL.md): `CALL <fn>`
- [`RET`](operands/RET.md): `RET`
- [`RETVAL`](operands/RETVAL.md): `RETVAL <value>`
- [`ARG`](operands/ARG.md): `ARG <value>`
- [`KWARG`](operands/KWARG.md): `KWARG <key>, <value>`
- [`ARG_ARITY`](operands/ARG_ARITY.md): `ARG_ARITY <dst>`
- [`KWARG_ARITY`](operands/KWARG_ARITY.md): `KWARG_ARITY <dst>`
- [`ARG_GET`](operands/ARG_GET.md): `ARG_GET <dst>, <index>`
- [`KWARG_GET`](operands/KWARG_GET.md): `KWARG_GET <dst>, <key>`
- [`KWARG_HAS`](operands/KWARG_HAS.md): `KWARG_HAS <dst>, <key>`
- [`ARG_REQUIRE`](operands/ARG_REQUIRE.md): `ARG_REQUIRE <index>, <type>`
- [`KWARG_REQUIRE`](operands/KWARG_REQUIRE.md): `KWARG_REQUIRE <key>, <type>`

## List Operations

- [`LIST_NEW`](operands/LIST_NEW.md): `LIST_NEW <dst>, <size>`
- [`LIST_PUSH`](operands/LIST_PUSH.md): `LIST_PUSH <list>, <value>`
- [`LIST_FILL`](operands/LIST_FILL.md): `LIST_FILL <list>, <count>, <value>`
- [`LIST_POP`](operands/LIST_POP.md): `LIST_POP <dst>, <list>`
- [`LIST_INSERT`](operands/LIST_INSERT.md): `LIST_INSERT <list>, <index>, <value>`
- [`LIST_ERASE`](operands/LIST_ERASE.md): `LIST_ERASE <list>, <index>`
- [`LIST_SLICE`](operands/LIST_SLICE.md): `LIST_SLICE <dst>, <list>, <start>`
- [`LIST_CLEAR`](operands/LIST_CLEAR.md): `LIST_CLEAR <list>`
- [`LIST_FIND`](operands/LIST_FIND.md): `LIST_FIND <dst>, <list>, <value>`
- [`LIST_SORT`](operands/LIST_SORT.md): `LIST_SORT <list>`
- [`LIST_REVERSE`](operands/LIST_REVERSE.md): `LIST_REVERSE <list>`
- [`LIST_GET`](operands/LIST_GET.md): `LIST_GET <dst>, <list>, <index>`
- [`LIST_SET`](operands/LIST_SET.md): `LIST_SET <list>, <index>, <value>`
- [`LIST_LEN`](operands/LIST_LEN.md): `LIST_LEN <dst>, <list>`
- `LIST_MAP <dst>, <list>, <fn>`: calls `<fn>(value, index)` for each element and returns the mapped list.
- `LIST_FILTER <dst>, <list>, <fn>`: calls `<fn>(value, index)` and keeps truthy results.
- `LIST_REDUCE <dst>, <list>, <fn>, <initial>`: calls `<fn>(accumulator, value, index)` and returns the final accumulator.
- `LIST_CONCAT <dst>, <lhs>, <rhs>`: returns a new list containing both inputs.
- `LIST_CLONE <dst>, <list>`: copies a list value.
- `LIST_DESTRUCTURE <list>, <dst>...`: writes list elements into destination slots in order.
- `LIST_VALIDATE <dst>, <list>, <type>`: returns whether every element matches a type expression.
- `LIST_ASSERT <list>, <type>`: throws unless every element matches a type expression.

## Map Operations

- [`MAP_NEW`](operands/MAP_NEW.md): `MAP_NEW <dst>`
- [`MAP_SET`](operands/MAP_SET.md): `MAP_SET <map>, <key>, <value>`
- [`MAP_GET`](operands/MAP_GET.md): `MAP_GET <dst>, <map>, <key>`
- [`MAP_HAS`](operands/MAP_HAS.md): `MAP_HAS <dst>, <map>, <key>`
- [`MAP_DELETE`](operands/MAP_DELETE.md): `MAP_DELETE <map>, <key>`
- [`MAP_KEYS`](operands/MAP_KEYS.md): `MAP_KEYS <dst>, <map>`
- [`MAP_VALUES`](operands/MAP_VALUES.md): `MAP_VALUES <dst>, <map>`
- `MAP_MERGE <dst>, <lhs>, <rhs>`: returns a merged map where right-side keys override left-side keys.
- `MAP_ENTRIES <dst>, <map>`: returns a list of `[key, value]` entry lists.
- `MAP_VALIDATE <dst>, <map>, <key_type>, <value_type>`: returns whether every key/value matches the supplied type expressions.

## Set Operations

- [`SET_NEW`](operands/SET_NEW.md): `SET_NEW <dst>`
- [`SET_ADD`](operands/SET_ADD.md): `SET_ADD <set>, <value>`
- [`SET_HAS`](operands/SET_HAS.md): `SET_HAS <dst>, <set>, <value>`
- [`SET_DELETE`](operands/SET_DELETE.md): `SET_DELETE <set>, <value>`
- [`SET_UNION`](operands/SET_UNION.md): `SET_UNION <dst>, <a>, <b>`
- [`SET_INTERSECT`](operands/SET_INTERSECT.md): `SET_INTERSECT <dst>, <a>, <b>`
- `SET_DIFFERENCE <dst>, <lhs>, <rhs>`: returns values present in `lhs` but not `rhs`.
- `SET_SYMMETRIC_DIFF <dst>, <lhs>, <rhs>`: returns values present in exactly one input set.
- `SET_VALIDATE <dst>, <set>, <type>`: returns whether every set member matches a type expression.
- `SET_ASSERT <set>, <type>`: throws unless every set member matches a type expression.

## Struct and Interface Operations

- `STRUCT_DEF_NEW <dst>`: creates a mutable structure definition.
- `STRUCT_DEF_NAME <def>, <name>`: names a structure definition.
- `STRUCT_DEF_FIELD <def>, <name>, <type>`: adds a typed field.
- `STRUCT_DEF_FIELD_DEFAULT <def>, <name>, <type>, <default>`: adds a typed field with a default value.
- `STRUCT_DEF_FIELD_VISIBILITY <def>, <name>, <visibility>`: sets `public`, `private`, or `protected` field metadata.
- `STRUCT_DEF_FIELD_IMMUTABLE <def>, <name>, <bool>`: marks whether a field can be changed after construction.
- `STRUCT_DEF_METHOD <def>, <method>, <fn>`: binds a method name to a function.
- `STRUCT_DEF_VALIDATOR <def>, <fn>`: registers a constructor validation hook.
- `STRUCT_DEF_IMPLEMENT <def>, <interface>`: records and validates an interface/protocol implementation.
- `STRUCT_DEF_EXTEND <def>, <parent_def>`: copies parent fields, methods, and interfaces into a definition.
- `STRUCT_DEF_SEAL <def>`: seals a definition so instances can be created.
- `STRUCT_NEW <dst>, <def>`: creates an instance from defaults.
- `STRUCT_INIT <dst>, <def>, <value>...`: creates an instance with positional field values.
- `STRUCT_GET <dst>, <struct>, <field>`: reads a public field by name.
- `STRUCT_SET <struct>, <field>, <value>`: writes a public mutable field by name.
- `STRUCT_GET_I <dst>, <struct>, <index>`: reads a public field by index.
- `STRUCT_SET_I <struct>, <index>, <value>`: writes a public mutable field by index.
- `STRUCT_TYPEOF <dst>, <struct>`: returns the declared structure name.
- `STRUCT_IS <dst>, <struct>, <type>`: checks a structure's declared type.
- `STRUCT_COPY <dst>, <struct>`: copies a structure value.
- `STRUCT_EQ <dst>, <lhs>, <rhs>`: compares two structures.
- `STRUCT_CALL <dst>, <struct>, <method>`: calls a bound method with the struct as `arg0`.
- `STRUCT_FIELDS <dst>, <struct_or_def>`: returns field names.
- `STRUCT_FIELD_INFO <dst>, <struct_or_def>, <field>`: returns field metadata as a map.
- `STRUCT_METHODS <dst>, <struct_or_def>`: returns method names.
- `STRUCT_INTERFACES <dst>, <struct_or_def>`: returns implemented interface names.
- `STRUCT_IMPLEMENTS <dst>, <struct_or_def>, <interface>`: checks interface implementation.
- `STRUCT_COMPOSE <dst>, <lhs>, <rhs>`: copies missing fields from `rhs` into `lhs`.
- `STRUCT_DESTRUCTURE <struct>, <dst>...`: writes field values into destination slots in declaration order.
- `INTERFACE_NEW <dst>`: creates an interface/protocol value.
- `INTERFACE_NAME <interface>, <name>`: names an interface.
- `INTERFACE_METHOD <interface>, <method>`: adds a required method name.
- `INTERFACE_HAS <dst>, <interface>, <method>`: checks whether an interface requires a method.

## Vector Operations

- [`VEC_NEW`](operands/VEC_NEW.md): `VEC_NEW <dst>, <size>`
- [`VEC_PUSH`](operands/VEC_PUSH.md): `VEC_PUSH <vec>, <value>`
- [`VEC_GET`](operands/VEC_GET.md): `VEC_GET <dst>, <vec>, <index>`
- [`VEC_SET`](operands/VEC_SET.md): `VEC_SET <vec>, <index>, <value>`
- [`VEC_LEN`](operands/VEC_LEN.md): `VEC_LEN <dst>, <vec>`
- [`VEC_ADD`](operands/VEC_ADD.md): `VEC_ADD <dst>, <a>, <b>`
- [`VEC_SUB`](operands/VEC_SUB.md): `VEC_SUB <dst>, <a>, <b>`
- [`VEC_SCALE`](operands/VEC_SCALE.md): `VEC_SCALE <dst>, <vec>, <scale>`
- [`VEC_DOT`](operands/VEC_DOT.md): `VEC_DOT <dst>, <a>, <b>`
- [`VEC_CROSS`](operands/VEC_CROSS.md): `VEC_CROSS <dst>, <a>, <b>`
- [`VEC_MAG`](operands/VEC_MAG.md): `VEC_MAG <dst>, <value>`
- [`VEC_NORM`](operands/VEC_NORM.md): `VEC_NORM <dst>, <value>`
- [`VEC_PROJECT`](operands/VEC_PROJECT.md): `VEC_PROJECT <dst>, <a>, <b>`
- [`VEC_REFLECT`](operands/VEC_REFLECT.md): `VEC_REFLECT <dst>, <v>, <normal>`
- [`VEC_ANGLE`](operands/VEC_ANGLE.md): `VEC_ANGLE <dst>, <a>, <b>`
- [`VEC_DISTANCE`](operands/VEC_DISTANCE.md): `VEC_DISTANCE <dst>, <a>, <b>`
- [`VEC_LERP`](operands/VEC_LERP.md): `VEC_LERP <dst>, <a>, <b>`

## Matrix Operations

- [`MAT_NEW`](operands/MAT_NEW.md): `MAT_NEW <dst>, <rows>, <cols>`
- [`MAT_GET`](operands/MAT_GET.md): `MAT_GET <dst>, <mat>, <index>`
- [`MAT_SET`](operands/MAT_SET.md): `MAT_SET <mat>, <index>, <value>`
- [`MAT_ROWS`](operands/MAT_ROWS.md): `MAT_ROWS <dst>, <mat>`
- [`MAT_COLS`](operands/MAT_COLS.md): `MAT_COLS <dst>, <mat>`
- [`MAT_MUL`](operands/MAT_MUL.md): `MAT_MUL <dst>, <a>, <b>`
- [`MAT_ADD`](operands/MAT_ADD.md): `MAT_ADD <dst>, <a>, <b>`
- [`MAT_SUB`](operands/MAT_SUB.md): `MAT_SUB <dst>, <a>, <b>`
- [`MAT_SCALE`](operands/MAT_SCALE.md): `MAT_SCALE <dst>, <mat>, <scale>`
- [`MAT_IDENTITY`](operands/MAT_IDENTITY.md): `MAT_IDENTITY <dst>, <size>`
- [`MAT_TRACE`](operands/MAT_TRACE.md): `MAT_TRACE <dst>, <value>`
- [`MAT_RREF`](operands/MAT_RREF.md): `MAT_RREF <dst>, <value>`
- [`MAT_RANK`](operands/MAT_RANK.md): `MAT_RANK <dst>, <value>`
- [`MAT_EIGEN`](operands/MAT_EIGEN.md): `MAT_EIGEN <dst>, <value>`
- [`MAT_SOLVE`](operands/MAT_SOLVE.md): `MAT_SOLVE <dst>, <matrix>, <vector>`
- [`MAT_TRANSPOSE`](operands/MAT_TRANSPOSE.md): `MAT_TRANSPOSE <dst>, <value>`
- [`MAT_DET`](operands/MAT_DET.md): `MAT_DET <dst>, <value>`
- [`MAT_INV`](operands/MAT_INV.md): `MAT_INV <dst>, <value>`

## String Operations

- [`STR_CONCAT`](operands/STR_CONCAT.md): `STR_CONCAT <dst>, <lhs>, <rhs>`
- [`STR_LEN`](operands/STR_LEN.md): `STR_LEN <dst>, <value>`
- [`STR_SUBSTR`](operands/STR_SUBSTR.md): `STR_SUBSTR <dst>, <lhs>, <rhs>`
- [`STR_FIND`](operands/STR_FIND.md): `STR_FIND <dst>, <lhs>, <rhs>`
- [`STR_REPLACE`](operands/STR_REPLACE.md): `STR_REPLACE <target>, <from>, <to>`
- [`STR_SPLIT`](operands/STR_SPLIT.md): `STR_SPLIT <dst>, <lhs>, <rhs>`
- [`STR_JOIN`](operands/STR_JOIN.md): `STR_JOIN <dst>, <lhs>, <rhs>`
- [`STR_UPPER`](operands/STR_UPPER.md): `STR_UPPER <dst>, <value>`
- [`STR_LOWER`](operands/STR_LOWER.md): `STR_LOWER <dst>, <value>`
- [`STR_TRIM`](operands/STR_TRIM.md): `STR_TRIM <dst>, <value>`
- [`STR_EQ`](operands/STR_EQ.md): `STR_EQ <dst>, <lhs>, <rhs>`
- [`REG_MATCH`](operands/REG_MATCH.md): `REG_MATCH <dst>, <text>, <pattern>`
- [`REG_FIND`](operands/REG_FIND.md): `REG_FIND <dst>, <text>, <pattern>`
- [`REG_REPLACE`](operands/REG_REPLACE.md): `REG_REPLACE <dst>, <text>, <pattern>, <replacement>`
- [`REG_GROUPS`](operands/REG_GROUPS.md): `REG_GROUPS <dst>, <text>, <pattern>`
- [`SQL_LOAD`](operands/SQL_LOAD.md): `SQL_LOAD <dst>, <rows>`
- [`SQL_QUERY`](operands/SQL_QUERY.md): `SQL_QUERY <dst>, <rows>, <query>`
- [`SQL_EXECUTE`](operands/SQL_EXECUTE.md): `SQL_EXECUTE <dst>, <rows>, <query>`
- [`JSON_STR_LOAD`](operands/JSON_STR_LOAD.md): `JSON_STR_LOAD <dst>, <json>`
- [`JSON_FILE_LOAD`](operands/JSON_FILE_LOAD.md): `JSON_FILE_LOAD <dst>, <path>`
- [`JSON_STR_DUMP`](operands/JSON_STR_DUMP.md): `JSON_STR_DUMP <dst>, <value>`
- [`JSON_FILE_DUMP`](operands/JSON_FILE_DUMP.md): `JSON_FILE_DUMP <path>, <value>`

## Type and Cast Operations

- [`TYPEOF`](operands/TYPEOF.md): `TYPEOF <dst>, <value>`
- [`TYPE_IS`](operands/TYPE_IS.md): `TYPE_IS <dst>, <value>, <type>`
- [`TYPE_ASSERT`](operands/TYPE_ASSERT.md): `TYPE_ASSERT <value>, <type>`
- [`TYPE_HINT`](operands/TYPE_HINT.md): `TYPE_HINT <slot>, <type>`
- [`TYPE_ALIAS`](operands/TYPE_ALIAS.md): `TYPE_ALIAS <name>, <type_expr>`
- [`CAST_INT`](operands/CAST_INT.md): `CAST_INT <dst>, <value>`
- [`CAST_FLOAT`](operands/CAST_FLOAT.md): `CAST_FLOAT <dst>, <value>`
- [`CAST_BOOL`](operands/CAST_BOOL.md): `CAST_BOOL <dst>, <value>`
- [`CAST_STRING`](operands/CAST_STRING.md): `CAST_STRING <dst>, <value>`
- [`IS_NULL`](operands/IS_NULL.md): `IS_NULL <dst>, <value>`
- [`IS_NAN`](operands/IS_NAN.md): `IS_NAN <dst>, <value>`
- [`IS_INF`](operands/IS_INF.md): `IS_INF <dst>, <value>`

## Time Operations

- [`TIME_NOW`](operands/TIME_NOW.md): `TIME_NOW <dst>`
- [`SLEEP`](operands/SLEEP.md): `SLEEP <seconds>`

## Memory and Slot Operations

- [`LOAD`](operands/LOAD.md): `LOAD <dst>, <src>`
- [`LOAD_RANGE`](operands/LOAD_RANGE.md): `LOAD_RANGE <start_slot_idx>, <end_slot_idx>, <value>`
- [`STORE`](operands/STORE.md): `STORE <dst>, <value>`
- [`SWAP`](operands/SWAP.md): `SWAP <a>, <b>`
- [`CLEAR`](operands/CLEAR.md): `CLEAR <target>`

## Random Operations

- [`RAND_RANDOM`](operands/RAND_RANDOM.md): `RAND_RANDOM <dst>`
- [`RAND_INT`](operands/RAND_INT.md): `RAND_INT <dst>, <min>, <max>`
- [`RAND_RANGE`](operands/RAND_RANGE.md): `RAND_RANGE <dst>, <min>, <max>`
