# Structures

`StructDef` and `Struct` provide user-defined record-like data in Axiom-IC bytecode.

A structure definition describes a named field layout. A structure instance stores values for that layout.


## Runtime Kinds

Structures use two runtime kinds:

- `ValueKind::StructDef` for structure definitions
- `ValueKind::Struct` for structure instances

General `TYPEOF` returns these runtime kind names:

- `StructDef`
- `Struct`

Use `STRUCT_TYPEOF` to get an instance's declared structure name, such as `Point`.


## Representation

Structure-related fields are stored directly on `Value`:

- `struct_name`: definition or instance type name
- `struct_sealed`: whether a definition is sealed
- `struct_field_names`: field names in declaration order
- `struct_field_types`: field type names in declaration order
- `struct_field_defaults`: default field values
- `struct_field_has_defaults`: whether a field has an explicit default
- `struct_values`: instance field values

`StructDef` values use the definition fields. `Struct` values copy the relevant name, field names, field types, and instance values.


## Definition Lifecycle

Definitions are built in phases:

```aic
STRUCT_DEF_NEW $0
STRUCT_DEF_NAME $0, #~"Point"
STRUCT_DEF_FIELD $0, #~"x", #~"Integer"
STRUCT_DEF_FIELD_DEFAULT $0, #~"y", #~"Integer", #0
STRUCT_DEF_SEAL $0
```

A definition is mutable until it is sealed.

After sealing:

- fields cannot be added
- the name cannot be changed
- the definition can be used by `STRUCT_NEW` and `STRUCT_INIT`

An unsealed definition cannot be instantiated.


## Field Types

Field types are stored as strings and checked at runtime when values are supplied.

Built-in type names include:

- `Integer`
- `Float`
- `String`
- `Boolean`
- `Null`
- `List`
- `Map`
- `Set`
- `StructDef`
- `Struct`
- `Vector`
- `Matrix`
- `Number`
- `Any`

`Number` accepts integers and floats. `Any` accepts every runtime value.

For structure values, a field type can also be a structure name. A `Struct` value matches its own `struct_name`, and also matches `Struct`.


## Construction

`STRUCT_NEW` creates an instance using defaults and `null` for missing defaults:

```aic
STRUCT_NEW $1, $0
```

`STRUCT_INIT` creates an instance and supplies positional field values:

```aic
STRUCT_INIT $1, $0, #3, #7
```

Values are assigned by declaration order. If fewer values are supplied than fields exist, the remaining fields keep their default or `null`.


## Field Access

Fields can be accessed by name:

```aic
STRUCT_GET $2, $1, #~"x"
STRUCT_SET $1, #~"x", #10
```

Fields can also be accessed by index:

```aic
STRUCT_GET_I $2, $1, #0
STRUCT_SET_I $1, #0, #10
```

Indexes follow field declaration order.


## Type Operations

Use `STRUCT_TYPEOF` to get the declared structure name:

```aic
STRUCT_TYPEOF $2, $1
```

Use `STRUCT_IS` to test a structure instance against a definition or string name:

```aic
STRUCT_IS $2, $1, $0
STRUCT_IS $3, $1, #~"Point"
```


## Copy and Equality

`STRUCT_COPY` copies a structure value:

```aic
STRUCT_COPY $2, $1
```

`STRUCT_EQ` compares two structure instances:

```aic
STRUCT_EQ $3, $1, $2
```

Current structure equality delegates to general VM value equality. For structures, that means equality is based on the structure string form.


## String Form

Structure definitions print like:

```text
structdef Point sealed {x: Integer, y: Integer = 0}
```

Structure instances print like:

```text
Point{x: 3, y: 7}
```


## Truthiness

Structure definitions are truthy when sealed.

Structure instances are always truthy.


## Supported Operations

Definition operations:

- `STRUCT_DEF_NEW`
- `STRUCT_DEF_NAME`
- `STRUCT_DEF_FIELD`
- `STRUCT_DEF_FIELD_DEFAULT`
- `STRUCT_DEF_SEAL`

Construction operations:

- `STRUCT_NEW`
- `STRUCT_INIT`

Field operations:

- `STRUCT_GET`
- `STRUCT_SET`
- `STRUCT_GET_I`
- `STRUCT_SET_I`

Type, copy, and comparison operations:

- `STRUCT_TYPEOF`
- `STRUCT_IS`
- `STRUCT_COPY`
- `STRUCT_EQ`


## Complete Example

```aic
.main
start:
  STRUCT_DEF_NEW $0
  STRUCT_DEF_NAME $0, #~"Point"
  STRUCT_DEF_FIELD $0, #~"x", #~"Integer"
  STRUCT_DEF_FIELD_DEFAULT $0, #~"y", #~"Integer", #7
  STRUCT_DEF_SEAL $0

  STRUCT_INIT $1, $0, #3
  STRUCT_GET $2, $1, #~"x"
  PRINTLN $2

  STRUCT_GET_I $3, $1, #1
  PRINTLN $3

  STRUCT_SET $1, #~"y", #9
  PRINTLN $1

  STRUCT_TYPEOF $4, $1
  PRINTLN $4

  STRUCT_IS $5, $1, $0
  PRINTLN $5

  STRUCT_COPY $6, $1
  STRUCT_EQ $7, $1, $6
  PRINTLN $7
  HALT
```

Output:

```text
3
7
Point{x: 3, y: 9}
Point
true
true
```


## Errors

Structure errors include:

- instantiating an unsealed definition
- sealing an unnamed definition
- modifying a sealed definition
- duplicate field names
- non-string field names or type names
- assigning a value that does not match the field type
- field name lookup failure
- field index out of bounds
- using a non-structure value where a structure is required


## Notes

- String operands use explicit AIC string immediates such as `#~"Point"`.
- Bare quoted strings such as `"Point"` are not valid operands.
- Definitions and instances are regular runtime values and can be stored in slots, passed as arguments, and placed inside containers.
