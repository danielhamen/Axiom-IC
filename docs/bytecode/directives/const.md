# `.const` Directive

Declares constants in the program constant pool.

## Section Form

```aic
.const
FLOAT 3.14159
STR ~"hello"
```

When `.const` appears alone, following constant declarations are appended to the constant pool until another directive starts.

## Named Pool Form

```aic
.const PrimaryConstant
  NULL
```

A named constant pool marks a range of constants with a symbolic pool name. Named pools are useful for module exports because `.export PrimaryConstant` can expose that pool to importing files.

Named pools must use section syntax. Inline named pool declarations are invalid:

```aic
.const MyConstantPool1 INT 3
```

This raises a parse error because inline `.const` declarations append a single value and cannot also open a named pool.

## Inline Form

```aic
.const FLOAT 3.14159
```

Inline constants append one value without opening a multi-line section.

## Indexed Form

```aic
.const 55 FLOAT 5.5
```

An explicit index writes the value at that constant-pool position, so it can be referenced as `@55`. Missing intermediate positions are filled with `NULL`.

## Supported Types

- `INT <integer>`
- `FLOAT <float>`
- `BOOL true|false`
- `NULL`
- `STR ~"text"`
