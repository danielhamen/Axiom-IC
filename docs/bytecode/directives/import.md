# `.import` Directive

Imports either an opcode category or an exported module symbol set.

## Opcode Category Imports

```aic
.import <arithmetic>
.import <control_flow>
```

Category imports enable opcodes in the named VM category. Instructions remain invalid until their category has been imported.

## Module Imports

```aic
.import Main
.import lib\utils
```

Module imports resolve a module by path relative to the importing file. The import path does not include `.aic`.

For example, from `my_program/Main.aic`:

```aic
.import lib\utils
```

resolves `my_program/lib/utils.aic`. That file must declare:

```aic
.module utils
```

The module declaration must match the imported file name.

## Selective Imports

```aic
.import Main{quotient, product}
```

Selective imports expose only the listed exported symbols to the importing file.

## Privacy

Files are private by default. A module import can only access names explicitly exported by the imported module with `.export`.

## Errors

An error is raised if:

- a category import references an unknown opcode category
- a module file does not exist
- an imported `.aic` file has no `.module` declaration
- the `.module` name does not match the imported file name
- a selective import requests a symbol that is not exported
- imported names conflict with local names
