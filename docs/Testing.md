# Testing

AxiomIC has a dependency-free repository test suite in `tests/run_tests.py`.

The suite uses `src/core/operations.cpp` as the operation registry and verifies that every registered opcode is represented in:

- `docs/bytecode/operands/<OPCODE>.md`
- `ext/zed/snippets/axiom-ic.json`
- `ext/zed/tree-sitter-axiom-ic/grammar.js`

It also runs all `.aic` files in `tests/fixtures/runtime` and compares their output to matching `.stdout` files.

## Run

```sh
python3 tests/run_tests.py --aic build/aic
```

After configuring with CMake:

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

## Add An Operation Test

Add a fixture:

```text
tests/fixtures/runtime/my_case.aic
tests/fixtures/runtime/my_case.stdout
```

The test runner executes the program, removes the `=== Running: ... ===` CLI banner, and compares stdout exactly.

Expected-failure tests can include:

```text
tests/fixtures/runtime/my_case.exit
tests/fixtures/runtime/my_case.stderr
```

The `.stderr` file is matched as a substring so diagnostics can include line/column context without making tests brittle.
