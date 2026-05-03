# Functions

Functions are named bytecode blocks with labels, local slots, positional arguments, keyword arguments, and optional return values.

Functions are not currently first-class runtime `Value` objects. They are stored in the program's function table and are referenced by name from `CALL`.


## Required Imports

All function operations are in the `function` category.

Any file using `CALL`, `RET`, `RETVAL`, `ARG`, `KWARG`, `ARG_GET`, `KWARG_GET`, `KWARG_HAS`, `ARG_ARITY`, `KWARG_ARITY`, `ARG_REQUIRE`, or `KWARG_REQUIRE` must import the category at the top of the file:

```aic
.import <function>
```

Imports must appear before `.const`, `.main`, `.fn`, labels, constants, or instructions.

Most complete programs also import `control_flow` for `HALT`, and often `io`, `memory`, or other categories depending on the instructions used.


## Declaration

The main entry point can be declared with `.main`:

```aic
.main
start:
  HALT
```

Named functions are declared with `.fn <name>`:

```aic
.fn add_one
  RET
```

Legacy directive-style function declarations are also supported, where any non-special directive becomes a function name:

```aic
.main
  HALT
```

Special directives are:

- `.import`
- `.const`
- `.include`
- `.fn`
- `.main`

`.include` is recognized but not implemented.


## Function Table

Parsed functions are stored in `Program::functions`.

Each `Function` stores:

- `ins`: instruction list
- `name`: function name
- `labels`: label name to program-counter mapping
- `arg_count`: minimum positional argument count inferred by static `argN` operands

Function names must be symbols and cannot be duplicated.


## Labels

Labels are local to a function:

```aic
start:
  JMP done
done:
  HALT
```

Labels are stored on the current function only. A jump in one function cannot target a label declared in another function.


## Call Frames

Each `CALL` creates a `CallFrame`.

A call frame stores:

- `return_pc`: caller program counter to resume at
- `return_fc`: caller function index to resume in
- `args`: positional argument values
- `kwargs`: keyword argument map
- `memory`: local slot storage for the callee
- `return_value`: pending return value
- `has_return_value`: whether `RETVAL` was used

The VM switches `fc` and `pc` to the callee function when the call begins.

When `RET` executes, the VM pops the frame and restores `return_fc` and `return_pc`.


## Slot Scope

Slots are call-frame local.

The `.main` function uses root program memory. Called functions use the active `CallFrame::memory`.

This means `$0` in a callee is not the same storage as `$0` in the caller:

```aic
.import <function>
.import <memory>
.import <io>
.import <control_flow>

.main
start:
  STORE $0, #10
  CALL change_local
  PRINTLN $0
  HALT

.fn change_local
  STORE $0, #99
  RET
```

Output:

```text
10
```


## Calling

`CALL <fn>` transfers execution to a named function:

```aic
CALL add
```

The function operand can be written as an identifier:

```aic
CALL add
```

or as a directive-style operand:

```aic
CALL .add
```

The target function must exist.


## Positional Arguments

The preferred calling convention uses `ARG` before `CALL`:

```aic
ARG #2
ARG #3
CALL add
```

The next `CALL` consumes all pending positional arguments into the callee call frame.

Inside the callee, fixed arguments can be read with static `argN` operands:

```aic
.fn add
  ADD $0, arg0, arg1
  RETVAL $0
  RET
```

Arguments can also be read dynamically:

```aic
ARG_GET $0, #0
```

Argument indexes are zero-based.


## Keyword Arguments

Keyword arguments are passed with `KWARG <key>, <value>` before `CALL`:

```aic
KWARG #~"name", #~"Ada"
CALL greet
```

Keys must be strings. The same pending keyword key cannot be set twice before a call.

Inside the callee:

```aic
KWARG_HAS $0, #~"name"
KWARG_GET $1, #~"name"
KWARG_ARITY $2
```

`KWARG_GET` raises an error if the key is missing. Use `KWARG_HAS` first when a key is optional.


## Pending Arguments

`ARG` and `KWARG` write to `Program::pending_args` and `Program::pending_kwargs`.

When `CALL` executes:

- if any pending positional or keyword arguments exist, the pending argument ABI is used
- pending positional arguments move into `CallFrame::args`
- pending keyword arguments move into `CallFrame::kwargs`
- pending argument storage is cleared

If pending arguments are used, the callee must receive at least its inferred `arg_count` positional arguments.


## Legacy Stack Arguments

If no pending `ARG` or `KWARG` values exist, `CALL` falls back to legacy stack arguments.

In this mode:

- the parser infers the callee's positional `arg_count` from static `argN` operands in the callee
- `CALL` takes that many values from the top of the stack
- those values become the callee's `args`
- the consumed values are removed from the caller stack

Example:

```aic
.import <function>
.import <stack>
.import <arithmetic>
.import <io>
.import <control_flow>

.main
start:
  PUSH #2
  PUSH #3
  CALL add
  POP $0
  PRINTLN $0
  HALT

.fn add
  ADD $0, arg0, arg1
  RETVAL $0
  RET
```

Output:

```text
5
```

New bytecode should prefer `ARG` and `KWARG`.


## Arity Helpers

`ARG_ARITY <dst>` stores the number of positional arguments in the active call frame:

```aic
ARG_ARITY $0
```

`KWARG_ARITY <dst>` stores the number of keyword arguments:

```aic
KWARG_ARITY $1
```

Both operations must be used inside a called function.


## Runtime Type Requirements

`ARG_REQUIRE <index>, <type>` validates that a positional argument exists and matches a type:

```aic
ARG_REQUIRE #0, #~"Integer"
```

`KWARG_REQUIRE <key>, <type>` validates that a keyword argument exists and matches a type:

```aic
KWARG_REQUIRE #~"name", #~"String"
```

Supported type names include:

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

Type requirements also support union expressions:

```aic
ARG_REQUIRE #0, #~"Integer|Float"
KWARG_REQUIRE #~"value", #~"Integer OR Float"
```

Both `|` and `OR` mean that any listed type is accepted.

For structure instances, the type name can also be the declared structure name.


## Return Values

`RETVAL <value>` stores a pending return value on the active call frame:

```aic
RETVAL $0
```

`RET` returns to the caller. If a return value was set, it is pushed onto the caller's stack:

```aic
CALL add
POP $0
```

Only one pending return value is stored. Calling `RETVAL` again before `RET` replaces the previous return value.

If `RET` runs with no active call frame, the program halts.

`RETVAL` is invalid outside a called function.


## Complete Example

```aic
.import <function>
.import <arithmetic>
.import <stack>
.import <io>
.import <control_flow>

.main
start:
  ARG #2
  ARG #3
  KWARG #~"label", #~"sum"
  CALL add
  POP $0
  PRINTLN $0
  HALT

.fn add
  ARG_REQUIRE #0, #~"Integer"
  ARG_REQUIRE #1, #~"Integer"
  KWARG_REQUIRE #~"label", #~"String"
  ADD $0, arg0, arg1
  RETVAL $0
  RET
```

Output:

```text
5
```


## Errors

Function-related errors include:

- calling a missing function
- using a non-function operand with `CALL`
- duplicate function declaration
- invalid function name
- not enough pending positional arguments
- not enough legacy stack arguments
- duplicate pending keyword argument keys
- reading a positional argument outside a call frame
- reading a positional argument index that does not exist
- reading a missing keyword argument
- using keyword argument helpers outside a call frame
- failing `ARG_REQUIRE` or `KWARG_REQUIRE`
- using `RETVAL` outside a called function
- using a function operation without `.import <function>` at the top of the file


## Supported Operations

Function operations:

- `CALL`
- `RET`
- `RETVAL`
- `ARG`
- `KWARG`
- `ARG_ARITY`
- `KWARG_ARITY`
- `ARG_GET`
- `KWARG_GET`
- `KWARG_HAS`
- `ARG_REQUIRE`
- `KWARG_REQUIRE`

Related stack operations:

- `PUSH`
- `POP`

`PUSH` and `POP` are in the `stack` category, not the `function` category.


## Notes

- Function arguments and return values are runtime `Value` objects, so any supported value type can be passed or returned.
- Function local slots are discarded when the function returns.
- Return values are transported through the caller's stack.
- Keyword argument storage uses `std::unordered_map<std::string, Value>`, so keyword order is not meaningful.
