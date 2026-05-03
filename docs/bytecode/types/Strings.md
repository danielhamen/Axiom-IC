# Strings

`String` stores textual byte data.


## Representation

- Runtime kind: `ValueKind::String`
- Storage field: `Value::s`
- C++ storage type: `std::string`
- Type name returned by `TYPEOF`: `String`


## Literals

String immediates use `#~"..."`:

```aic
STORE $0, #~"hello"
STORE $1, #~"She said \"Hello\""
```

String constants can also be declared in `.const`:

```aic
.const
  STR ~"hello"
```

Or as an anonymous inline constant:

```aic
.const STR ~"hello"
.const 12 STR ~"indexed" ; available as @12
```

Bare quoted strings such as `"hello"` are not valid operands.


## String Form

`Value::to_str()` returns the string contents directly, without quotes.

```aic
.main
start:
  PRINTLN #~"hello"
  HALT
```

Output:

```text
hello
```


## Truthiness

Strings are false only when empty.

- `#~""` is false
- any non-empty string is true


## Equality

Strings compare by exact `std::string` contents.

String equality is case-sensitive and byte-based.


## Supported Operations

String-specific operations:

- `STR_CONCAT`
- `STR_LEN`
- `STR_SUBSTR`
- `STR_FIND`
- `STR_REPLACE`
- `STR_SPLIT`
- `STR_JOIN`
- `STR_UPPER`
- `STR_LOWER`
- `STR_TRIM`
- `STR_EQ`

Strings are also used as:

- map keys for `MAP_SET`, `MAP_GET`, `MAP_HAS`, and `MAP_DELETE`
- file paths for file operations
- file contents for `WRITE_FILE` and `APPEND_FILE`


## Casts

- `CAST_STRING` returns the same string value.
- `CAST_INT` parses with `std::stoll`.
- `CAST_FLOAT` parses with `std::stod`.
- `CAST_BOOL` returns false only for the empty string.


## Errors

An error is raised when:

- a string operation receives a non-string operand where a string is required
- `STR_SUBSTR` receives an out-of-bounds start index
- `STR_JOIN` receives a list containing non-string values
- numeric casts from string fail


## Notes

- String indexing and length use `std::string` byte positions, not Unicode scalar values.
