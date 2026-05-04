# Modules

Modules let AxiomIC programs share functions and named constant pools across files.

## Standalone Files

A file without `.module` is standalone. It can run directly and can import modules, but it cannot be imported by another file.

```aic
; second.aic
.import Main
```

The imported file must contain `.module Main`; otherwise import resolution fails.

## Module Files

```aic
; Main.aic
.module Main
.import <function>

.fn sum
  RET #4

.export sum
```

The file name and module declaration must match. `Main.aic` must declare `.module Main`.

## Import Paths

Imports are relative to the importing file:

```text
my_program/
  Main.aic
  lib/
    utils.aic
```

`my_program/lib/utils.aic` can be imported from `Main.aic` with:

```aic
.import lib\utils
```

The imported file declares only the file stem:

```aic
.module utils
```

## Selective Imports

```aic
.import Main{quotient, product}
```

Only `quotient` and `product` become visible in the importing file. Any other exported symbols from `Main` stay unimported.

## Privacy

Functions and named constant pools are private by default. Use `.export` to make a symbol importable.

```aic
.fn product
  RET

.export product
```

Private helper functions can still be used inside their own module by exported functions.

## Named Constant Pools

Named pools use section syntax:

```aic
.const PrimaryConstant
  NULL
  STR ~"ready"

.export PrimaryConstant
```

Inline named constant pools are invalid:

```aic
.const PrimaryConstant NULL
```

## Linking Model

When a program is loaded, module imports are resolved before verification and execution. Imported module functions are linked under internal names, and references to imported exported functions are rewritten during linking.

An import error stops execution before bytecode verification.
