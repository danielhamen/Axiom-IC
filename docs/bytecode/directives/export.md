# `.export` Directive

Exports a module symbol for other files to import.

## Syntax

```aic
.export <symbol>
```

## Exportable Symbols

The symbol can name:

- a function declared with `.fn` or shorthand function directives
- a named constant pool declared with `.const <PoolName>`

## Behavior

Everything in a module is private by default. `.export` marks a symbol as public so another file can import it with `.import ModuleName` or `.import ModuleName{symbol}`.

## Example

```aic
.module Main

.fn sum
  RET #4

.export sum
```

## Errors

An error is raised if:

- the export line has no symbol
- the symbol is exported more than once
- the exported symbol is neither a function nor a named constant pool
