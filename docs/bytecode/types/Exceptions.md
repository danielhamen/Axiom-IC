# Exceptions

Axiom-IC exceptions are runtime `Error` values. An error has:

- `type`: a string such as `RuntimeError`, `TypeError`, `ValueError`, `IndexError`, `KeyError`, `NameError`, or `ZeroDivisionError`
- `message`: a human-readable explanation

Custom error types are regular strings created with `ERROR_NEW`.


## Required Import

All exception operations are in the `exception` category:

```aic
.import <exception>
```


## Creating Errors

```aic
ERROR_NEW $0, #~"ValidationError", #~"expected a positive value"
THROW $0
```

Throwing a non-`Error` value wraps it in an `Error` whose type is the value kind.


## Handling Errors

`TRY <label>` installs a handler for the current function. If an error is thrown before `END_TRY`, execution jumps to the label and the error is available through `ERR_GET`.

```aic
TRY failed
DIV $0, #1, #0
END_TRY
JMP done

failed:
  CATCH
  ERR_GET $1
  ERROR_TYPE $2, $1
  ERROR_MESSAGE $3, $1
  ERR_CLEAR

done:
  HALT
```

`CATCH` marks handler code and is a no-op at runtime. `ERR_CLEAR` clears the current handled error.


## Standard Errors

The runtime raises standard error types for common failures:

- `RuntimeError`: general VM/runtime failures
- `TypeError`: operand and value kind mismatches
- `ValueError`: invalid values or invalid type expressions
- `IndexError`: out-of-bounds indexes
- `KeyError`: missing map or keyword keys
- `NameError`: missing functions, labels, methods, or symbols
- `ZeroDivisionError`: division or modulo by zero

Uncaught errors still abort execution, but they are rendered through the same diagnostic context as other VM errors: function name, opcode, and program counter.


## Finally Blocks

`FINALLY <label>` attaches a finally target to the active handler. If an error is thrown while the handler is active, the VM jumps to the finally label first. The current error remains available via `ERR_GET`, so finally code can inspect or rethrow it.

