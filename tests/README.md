# AxiomIC Tests

Run the repository test suite with:

```sh
python3 tests/run_tests.py --aic build/aic
```

or through CTest after configuring CMake:

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

## Runtime Fixtures

Execution tests are data files in `tests/fixtures/runtime`.

For a passing program, add:

- `name.aic`
- `name.stdout`

The runner executes `name.aic`, removes the CLI banner line, and compares stdout exactly.

For expected failures, also add:

- `name.exit` with the expected process exit code
- optional `name.stderr` containing a substring that must appear in stderr

This keeps operation behavior tests cheap to add and easy to review.

## Coverage Tests

The test runner reads `src/core/operations.cpp` as the source of truth and checks that every registered operation has:

- an operand documentation page in `docs/bytecode/operands`
- a Zed snippet template
- a Zed grammar entry

It also checks directive docs, import snippets, constant snippets, and basic documentation quality.
