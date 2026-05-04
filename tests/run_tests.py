#!/usr/bin/env python3
"""Repository tests for AxiomIC.

The suite is intentionally data driven:
- operation metadata is read from src/core/operations.cpp
- documentation/snippet/grammar coverage is checked from that registry
- runtime behavior cases live in tests/fixtures/runtime/*.aic

To add an execution test, add a .aic file and a matching .stdout file.
Optional .stderr and .exit files can assert stderr and non-zero exits.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
import unittest
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_AIC = ROOT / "build" / "aic"
OPERATIONS_CPP = ROOT / "src" / "core" / "operations.cpp"
OPERANDS_DOC_DIR = ROOT / "docs" / "bytecode" / "operands"
DIRECTIVES_DOC_DIR = ROOT / "docs" / "bytecode" / "directives"
ZED_SNIPPETS = ROOT / "ext" / "zed" / "snippets" / "axiom-ic.json"
ZED_ROOT_SNIPPETS = ROOT / "ext" / "zed" / "snippets.json"
ZED_GRAMMAR = ROOT / "ext" / "zed" / "tree-sitter-axiom-ic" / "grammar.js"
RUNTIME_FIXTURES = ROOT / "tests" / "fixtures" / "runtime"


@dataclass(frozen=True)
class OperationDef:
    kind: str
    name: str
    arity: int | None
    category: str


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def operation_defs() -> list[OperationDef]:
    text = read_text(OPERATIONS_CPP)
    pattern = re.compile(
        r'\{OperationKind::([A-Z0-9_]+),\s*"([A-Z0-9_]+)",\s*([^,]+),\s*"([^"]+)"\}'
    )
    defs: list[OperationDef] = []
    for kind, name, arity_text, category in pattern.findall(text):
        arity_text = arity_text.strip()
        arity = None if arity_text == "kVariadicArity" else int(arity_text)
        defs.append(OperationDef(kind=kind, name=name, arity=arity, category=category))
    return defs


def load_json(path: Path) -> dict:
    with path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def normalized_stdout(stdout: str) -> str:
    lines = [line for line in stdout.splitlines() if not line.startswith("=== Running: ")]
    return "\n".join(lines).rstrip() + ("\n" if lines else "")


class RegistryTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.ops = operation_defs()
        cls.by_name = {op.name: op for op in cls.ops}

    def test_operation_registry_is_parseable_and_unique(self) -> None:
        self.assertGreater(len(self.ops), 0, "No operation definitions were discovered")
        self.assertEqual(len(self.ops), len(self.by_name), "Operation names must be unique")
        for op in self.ops:
            self.assertRegex(op.category, r"^[a-z][a-z0-9_]*$")
            if op.arity is not None:
                self.assertGreaterEqual(op.arity, 0)

    def test_every_operation_has_operand_documentation(self) -> None:
        missing = [op.name for op in self.ops if not (OPERANDS_DOC_DIR / f"{op.name}.md").exists()]
        self.assertEqual(missing, [], "Missing operand docs for registered operations")

    def test_operand_docs_have_basic_supporting_content(self) -> None:
        failures: list[str] = []
        for op in self.ops:
            path = OPERANDS_DOC_DIR / f"{op.name}.md"
            text = read_text(path)
            if op.name not in text:
                failures.append(f"{path}: missing operation name")
            if "```" not in text and "Syntax" not in text and "Behavior" not in text:
                failures.append(f"{path}: missing syntax/behavior section")
            if re.search(r"\bTODO\b|\bTBD\b", text, re.IGNORECASE):
                failures.append(f"{path}: contains TODO/TBD placeholder")
        self.assertEqual(failures, [], "Operand docs need supporting content")

    def test_operand_docs_do_not_reference_unknown_operations(self) -> None:
        known = set(self.by_name)
        stale = []
        for path in OPERANDS_DOC_DIR.glob("*.md"):
            if path.stem.isupper() and path.stem not in known:
                stale.append(path.name)
        self.assertEqual(stale, [], "Operand docs exist for unknown operations")

    def test_directive_docs_cover_current_directives(self) -> None:
        for directive in ("const", "export", "fn", "import", "main", "module"):
            self.assertTrue((DIRECTIVES_DOC_DIR / f"{directive}.md").exists())
        const_doc = read_text(DIRECTIVES_DOC_DIR / "const.md")
        self.assertIn(".const FLOAT 3.14159", const_doc)
        self.assertIn(".const 55 FLOAT 5.5", const_doc)
        self.assertIn(".const PrimaryConstant", const_doc)


class ExtensionCoverageTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.ops = operation_defs()
        cls.snippets = load_json(ZED_SNIPPETS)
        cls.root_snippets = load_json(ZED_ROOT_SNIPPETS)
        cls.grammar = read_text(ZED_GRAMMAR)

    def test_snippet_files_are_in_sync(self) -> None:
        self.assertEqual(self.snippets, self.root_snippets)

    def test_every_operation_has_a_zed_snippet(self) -> None:
        missing = [op.name for op in self.ops if op.name not in self.snippets]
        self.assertEqual(missing, [], "Missing Zed snippets for registered operations")

    def test_operation_snippets_expand_to_templates(self) -> None:
        failures: list[str] = []
        for op in self.ops:
            snippet = self.snippets[op.name]
            body = snippet.get("body", "")
            if snippet.get("prefix") != op.name:
                failures.append(f"{op.name}: prefix mismatch")
            if not body.startswith(op.name):
                failures.append(f"{op.name}: body does not start with opcode")
            if op.arity is not None and op.arity > 0:
                for slot in range(1, op.arity + 1):
                    if f"${{{slot}:" not in body:
                        failures.append(f"{op.name}: missing tab stop {slot}")
            if "$0" not in body:
                failures.append(f"{op.name}: missing final cursor tab stop")
        self.assertEqual(failures, [], "Opcode snippets must be usable templates")

    def test_every_operation_is_in_zed_grammar(self) -> None:
        missing = [op.name for op in self.ops if f'"{op.name}"' not in self.grammar]
        self.assertEqual(missing, [], "Missing operations from Zed grammar")

    def test_every_category_has_an_import_snippet(self) -> None:
        categories = sorted({op.category for op in self.ops})
        missing = [category for category in categories if f"import {category}" not in self.snippets]
        self.assertEqual(missing, [], "Missing category import snippets")

    def test_directive_and_constant_snippets_exist(self) -> None:
        for key in (
            "directive import",
            "directive import module",
            "directive import selective",
            "directive main",
            "directive module",
            "directive export",
            "directive fn",
            "directive const section",
            "directive const named",
            "const FLOAT",
            "const indexed FLOAT",
            "const STR",
            "const indexed STR",
        ):
            self.assertIn(key, self.snippets)


class RuntimeFixtureTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.aic = Path(os.environ.get("AIC_BINARY", str(DEFAULT_AIC))).resolve()
        override = getattr(cls, "AIC_BINARY_OVERRIDE", None)
        if override is not None:
            cls.aic = Path(override).resolve()
        if not cls.aic.exists():
            raise unittest.SkipTest(f"AxiomIC binary not found: {cls.aic}")

    def test_runtime_fixtures(self) -> None:
        fixtures = sorted(RUNTIME_FIXTURES.glob("*.aic"))
        self.assertGreater(len(fixtures), 0, "No runtime fixtures found")
        for fixture in fixtures:
            with self.subTest(fixture=fixture.name):
                expected_stdout = read_text(fixture.with_suffix(".stdout")) if fixture.with_suffix(".stdout").exists() else ""
                expected_stderr = read_text(fixture.with_suffix(".stderr")) if fixture.with_suffix(".stderr").exists() else ""
                expected_exit = 0
                exit_file = fixture.with_suffix(".exit")
                if exit_file.exists():
                    expected_exit = int(read_text(exit_file).strip())

                proc = subprocess.run(
                    [str(self.aic), str(fixture)],
                    cwd=self.aic.parent,
                    text=True,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    check=False,
                )

                self.assertEqual(proc.returncode, expected_exit, proc.stderr)
                self.assertEqual(normalized_stdout(proc.stdout), expected_stdout)
                if expected_stderr:
                    self.assertIn(expected_stderr.strip(), proc.stderr)

    def test_exported_named_constant_pool_is_linked(self) -> None:
        fixture = RUNTIME_FIXTURES / "module_named_pool.aic"
        proc = subprocess.run(
            [str(self.aic), "--constants", str(fixture)],
            cwd=self.aic.parent,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertIn(".const PrimaryConstant start=@", proc.stdout)

    def test_optimizer_rewrites_bytecode(self) -> None:
        fixture = RUNTIME_FIXTURES / "optimizer.aic"
        proc = subprocess.run(
            [str(self.aic), "--bytecode", str(fixture)],
            cwd=self.aic.parent,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
        self.assertEqual(proc.returncode, 0, proc.stderr)
        self.assertIn("Instruction(op=LOAD", proc.stdout)
        self.assertIn("immediate=5", proc.stdout)
        self.assertNotIn("Instruction(op=ADD", proc.stdout)
        self.assertNotIn("Instruction(op=JMP,", proc.stdout)
        self.assertRegex(proc.stdout, r"Instruction\(op=JMP_IF, operands=\[Operand\(kind=Label, value=done, resolved=\d+\)")
        self.assertNotIn("Operand(kind=Slot, value=0), Operand(kind=Slot, value=0)", proc.stdout)
        self.assertNotIn("unreachable", proc.stdout)
        self.assertRegex(proc.stdout, r"Instruction\(op=CALL, operands=\[Operand\(kind=Function, value=product, resolved=\d+\)\]\)")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--aic", default=None, help="Path to the aic executable")
    args, unittest_args = parser.parse_known_args()

    if args.aic:
        RuntimeFixtureTests.AIC_BINARY_OVERRIDE = args.aic

    unittest.main(argv=[sys.argv[0], *unittest_args], verbosity=2)
    return 0


if __name__ == "__main__":
    main()
