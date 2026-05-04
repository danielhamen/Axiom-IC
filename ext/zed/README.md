# Axiom-IC Zed Extension

Language support for Axiom-IC bytecode in Zed.

## Features

- Associates `.aic` files with the `Axiom-IC` language.
- Provides a Tree-sitter grammar for directives, module imports/exports, labels, constants, instructions, and operands.
- Highlights all currently registered operations from `src/core/operations.cpp`.
- Highlights slots, immediates, constants, addresses, strings, numbers, booleans, nulls, labels, comments, and function declarations.
- Provides snippets for every current operation with tab-stop operands, plus directive, import, and constant declaration snippets.

## Install Locally

In Zed, run `zed: install dev extension` and select this directory:

```text
ext/zed
```

If Zed does not load the grammar, open `Zed.log` with `zed: open log`. The grammar is referenced through a local `file://` repository entry in `extension.toml`; because Zed fetches grammars with Git, the local grammar directory must have a commit and `rev` must point at that commit.

## AIC String Syntax

Immediate strings use AIC's explicit string literal syntax:

```aic
MAP_SET $0, #~"answer", $1
```

Bare quoted strings are intentionally not parsed as string operands:

```aic
MAP_SET $0, "answer", $1
```

## Maintenance

The operation grammar and snippets should stay aligned with `src/core/operations.cpp`. After changing the grammar, regenerate the parser:

```sh
cd ext/zed/tree-sitter-axiom-ic
npx tree-sitter-cli generate
git add grammar.js package.json tree-sitter.json src
git commit -m "Update Axiom-IC grammar"
git rev-parse HEAD
```

Then update the grammar `rev` in `extension.toml` to the new commit hash.

Useful smoke checks:

```sh
cd ext/zed/tree-sitter-axiom-ic
npx tree-sitter-cli parse ../../../build/Main.aic
npx tree-sitter-cli query ../languages/axiom-ic/highlights.scm ../../../build/Main.aic
```

## References

- Zed language extension docs: https://zed.dev/docs/extensions/languages
- Zed extension development docs: https://zed.dev/docs/extensions/developing-extensions
- Zed snippets docs: https://zed.dev/docs/snippets
