# `.module` Directive

Declares that a file is an importable module.

## Syntax

```aic
.module <moduleName>
```

## Behavior

Files without `.module` are standalone programs. They can import modules, but other files cannot import them as modules.

The module name must match the file name. A file named `Main.aic` must declare:

```aic
.module Main
```

A file named `lib/utils.aic` must declare:

```aic
.module utils
```

and can be imported as:

```aic
.import lib\utils
```

## Placement

`.module` must appear at top level before functions or constants. It can appear before category imports or module imports.

## Errors

An error is raised if:

- a module is imported from a file with no `.module` declaration
- the module name does not match the file stem
- a file declares more than one module
- `.module` appears after functions or constants
