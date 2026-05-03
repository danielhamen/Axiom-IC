# TYPE_ALIAS

`TYPE_ALIAS <name>, <type_expr>`

Creates a type-expression alias.

Requires `.import <type>`.


## Behavior

The first operand is an identifier. The second operand must resolve to a string type expression.

```aic
TYPE_ALIAS NonAddable, #~"Boolean|Integer"
TYPE_ASSERT $0, NonAddable
```

Aliases can be used anywhere the VM reads a type expression operand, including `TYPE_IS`, `TYPE_ASSERT`, `TYPE_HINT`, argument requirements, collection validation, and struct field declarations.

Aliases are expanded when they are defined. Redefining an alias replaces the previous mapping and emits a warning.

