# Testing

How the C~~ test suite is structured, why, and the roadmap for building it.
End-to-end compiler tests (compile a `.c` file, run it, check the exit code) are also covered in `educational/followup-setup-testing-expectations.md`. This document focuses on unit tests and the overall setup.

Decisions: ADR-002 (commit-time test check) and ADR-003 (golden-file tests) in `architecture-decisions.md`.

---

## ELI5

A unit test is a tiny program that checks one small piece of the code: give it a known input and check it gets the expected answer. For example: "lexing `==;` should give `==`, then `;`, then EOF."

There are hundreds of these, and one command runs all of them in about a second. After every change, run them. If something that used to work breaks, a test goes red right away and says *which* case broke, instead of it being discovered weeks later while debugging generated assembly.

It's the difference between re-checking everything by hand after each change, and having a robot recheck all of it every time.

---

## Kinds of tests

| Kind | What it checks | Example | Speed | When it fails, it tells you… |
|---|---|---|---|---|
| **Unit** | One module on its own | `List` grows past capacity; `lex("==;")` gives the right tokens | Microseconds | *exactly* which function and case |
| **Integration** | Several modules together | Source string → lexer → parser → AST shape | Fast | which stage boundary |
| **End-to-end** | The whole compiler plus gcc plus running the result | `return 42;` → executable exits with status 42 | Slow (processes, files) | "something somewhere is wrong" |

This is the **test pyramid**: many unit tests at the bottom, fewer integration tests, and a smaller set of end-to-end tests on top. For a compiler, end-to-end tests matter more than usual, because "does the generated program behave correctly?" is the real goal. While building the lexer and parser, though, unit tests are what catch bugs early.

---

## Anatomy of a test

Every test follows **Arrange / Act / Assert**:
1. **Arrange:** set up the input (a source string, an empty list).
2. **Act:** call the one thing under test.
3. **Assert:** check the result (token kinds, ranges, list size, returned value).

A good test is:
- **Small.** It checks one behavior, and its name says which: `lex_punctuator_stops_at_eof`, not `test3`.
- **Independent.** It doesn't depend on other tests having run, and shares no state with them. Each test builds its own `Context`.
- **Deterministic.** Same result every run. No time, randomness or environment dependence.
- **Fast.** If the suite takes a minute, it stops getting run.
- **Seen failing first.** Run it before the fix and watch it go red. A test that has never failed may not be testing anything.

## Choosing test cases

- **Boundaries** are where bugs live: empty input, one element, exactly the maximum, the maximum plus one. The early lexer bugs were all boundary bugs: EOF, the 4095th byte, list capacity 0→1, a lone `+` at the end of the file.
- **One test per rule in SUBSET.md:** every punctuator, every keyword, every escape sequence.
- **Error cases:** `123abc`, `''`, an unterminated string. Check that the right diagnostic is produced and that the lexer recovers.
- **Every bug gets a regression test**, written *before* the fix, so it can never come back unnoticed.

---

## Building the suite

### 1. The compiler as a library

A test program needs its own `main`, so it can't link against an executable that already has one. The standard structure:

```
code/
  src/ ...          → add_library(c_compiler_core ...)   (everything except main.c)
  main.c            → add_executable(c_compiler main.c)        links c_compiler_core
  tests/
    test_list.c     → add_executable(test_list ...)            links c_compiler_core
    test_lexer.c    → add_executable(test_lexer ...)           links c_compiler_core
```

Then `enable_testing()` plus one `add_test(NAME lexer COMMAND test_lexer)` per test executable. That's **CTest**, CMake's test runner: it runs each executable and treats **exit code 0 as pass, anything else as fail**. CLion shows CTest results as a green/red tree.

### 2. Test through the public interface

`static` functions like `lexer_lex_punctuator` can't be called from a test file, and that's fine. **Test behavior, not internals:** call `lexer_lex_source_file` on `"==;"` and check the tokens. Such tests survive refactoring; tests that reach into private functions break on every reorganization. (Don't `#include "Lexer.c"` into a test.)

### 3. Testability shapes the design

Two changes the tests will require, both improvements anyway:
- **Lexing from a string.** A `SourceFile` can currently only come from a file path. Tests want `lex("==;")` with no files on disk, so there needs to be a way to build a `SourceFile`/`Context` from an in-memory buffer. Decide deliberately whether that buffer gets a `'\0'` sentinel after the text.
- **Diagnostics that can be inspected.** If diagnostics only go to `stderr`, a test can't check "`123abc` produced `INVALID_NUMERICAL_SEQUENCE` at 1:1". If they're *collected* (e.g. in `Context`), the test can inspect the list.

Code that is hard to test is usually badly structured. Writing tests shows where.

### 4. The harness

- **Hand-rolled first.** A `CHECK(cond)` macro that, on failure, prints `__FILE__:__LINE__` and the expression text, increments a failure counter, and **does not abort**. A `main` that calls every test function and returns non-zero if anything failed. About 30 lines, and afterwards it's clear what every framework does. This is a legitimate exception to the project's "no macros" guideline: only a macro can capture `__FILE__`, `__LINE__` and the expression text (`#cond`).
- **Frameworks later:** Unity, µnit, CMocka, Criterion (see `research.md`). All work with CTest.

**Don't use `assert()` for test checks.** It stops at the first failure, so each run shows only one problem, and under `NDEBUG` it disappears and every test "passes".

---

## Golden-file tests (chosen, ADR-003)

A golden-file (snapshot) test runs a stage on an input file, dumps the output as text, and compares it to a checked-in **expected** file. A difference means a failure, and the diff shows exactly what changed.

This fits compilers well: stage outputs (tokens, later the AST, later assembly) are large and structured, and asserting on them field by field in C is verbose. The token-dump `main` from `TODO.md` is most of the machinery already.

### Layout

```
tests/golden/lexer/
  punctuators.c          # input
  punctuators.expected   # expected token dump
  eof_after_plus.c
  eof_after_plus.expected
  ...
```

A small runner (a script, or a C test program) runs the compiler in dump mode on each `.c`, compares against its `.expected`, and reports each file as pass/fail. It's registered with CTest like the unit tests.

### The dump format

The dump is the contract, so it must be:
- **Deterministic:** no pointer addresses, no timestamps, nothing that varies between runs or machines.
- **Stable:** one token per line, fixed field order, e.g. `1:5  PUNCT  ==`. Changing the format rewrites every expected file, so settle it early.
- **Readable:** a human reviews these files, so they should be readable when diffed.
- **Complete enough:** include the fields that matter (kind, spelling, line:column, value/id where relevant).

### Workflow

- **New test:** write the input, run the dump, **check the output by hand against the spec**, then save it as `.expected`. The expected file is only as correct as that manual check.
- **Intentional change** (e.g. a new token kind): the tests go red, review every diff, and if every change is intended, regenerate the expected files. An "update expected files" mode in the runner makes this one command.
- **Unintended change:** that's a bug the test caught.

### Gotchas

- **Never bulk-accept diffs without reading them.** That's how a snapshot suite silently turns into "whatever the code does now".
- **Line endings:** git runs on Windows, and `core.autocrlf` can silently turn `.expected` files into CRLF while the compiler on Linux prints LF, so every test fails. Pin it with `.gitattributes` (e.g. `*.expected text eol=lf` and the same for test inputs).
- **Trailing newline / whitespace** at the end of the dump: decide once, and make the comparison exact.
- Golden files complement unit tests, they don't replace them. A unit test names *the one* behavior it checks; a golden file checks everything at once and needs a human to interpret the diff.

---

## Automation

| When | Mechanism | Blocks on failure? | Notes |
|---|---|---|---|
| Every build | CMake `POST_BUILD` custom command that runs the tests | ✅ The build fails | A failing test blocks building the binary needed to debug it. Not used. |
| Before running the program | CLion Run Configuration → **Before launch** → "Run Another Configuration" → CTest configuration | ✅ Launch is cancelled (verify once by breaking a test on purpose) | Gets in the way while debugging. Not used. |
| **Before commit, in CLion** ✅ **chosen** | Commit dialog → gear icon → commit checks → **"Run Tests"** with the CTest configuration (option names vary by CLion version) | ✅ Warns or blocks | Runs through CLion, so it uses the **remote Ubuntu toolchain**. |
| Before commit, in git | `pre-commit` hook script; non-zero exit aborts the commit | ✅ Unless bypassed with `--no-verify` | git runs on **Windows** but the tests need the **Linux** build, so the hook would have to call into it (`wsl …`/`ssh …`). `.git/hooks/` isn't versioned; use `core.hooksPath`. Not used. |
| After push | CI (GitHub Actions) | ❌ Doesn't block, but marks the commit red | Clean Linux machine. Possible later addition. |

**Decision (ADR-002):** the CLion before-commit check, plus running the tests by hand constantly (CTest tree view in CLion). Commits made outside CLion skip the check; CI can cover that later.

---

## Tips and gotchas

- **Run the tests under sanitizers.** A test that passes while overflowing a buffer passes by luck. Add an ASan/UBSan build configuration and run the same suite in it; `ctest -T memcheck` runs it under Valgrind (see `research.md`).
- **Clean up in tests too.** Every test destroys what it created, or LeakSanitizer reports noise from the tests that hides real leaks.
- **Test counts lie; coverage helps.** `gcov`/`lcov` or CLion's "Run with Coverage" shows which lines never ran. Aim for "every branch in the lexer has run at least once", not a percentage (see `research.md`).
- **Don't repeat the implementation in the test.** If the expected value comes from the same logic as the code, both are wrong together. Write expected values by hand from the *spec* (SUBSET.md, the standard), not from the code.
- **When a test fails, suspect the test too.** But never "fix" a test by changing the expected value until it passes without understanding why it failed.
- **Asserts are hard to test in C:** checking that one fires means crashing the process (frameworks use `fork()`). Skip it for now. Tests exercise *correct* behavior and *user errors* (diagnostics).
- **Start small.** Five lexer tests that actually run beat a perfect framework that doesn't exist yet.

---

## Roadmap

Built step by step. Each step ends in a state that works and can be committed.

**Step 0: Prerequisites**
- [x] A way to create a `SourceText`/`Context` from an in-memory string (`source_text_create_from_buffer` + `context_create`).

The real `main` was moved from here to step 7: the harness comes first, so bugs show up one function at a time in unit tests, not all at once through `main`.

**Step 1: Library split**
- [x] Move everything except `main.c` into a CMake library; `main` links it.
- [x] The compiler still builds and runs exactly as before.

**Step 2: Hand-rolled harness**
- [x] `CHECK` macro(s) and a runner `main` that returns non-zero on any failure.
- [x] One trivial test. Watch it fail (break it on purpose), then pass.

**Step 3: CTest + CLion**
- [x] `enable_testing()` and `add_test` for the test executable.
- [x] Tests appear in CLion's test tree; run them from there.

**Step 4: First real unit tests**, one sub-step per test executable.

**Step 4a: `List`** (`test_list`)
- [x] Push past capacity, pop from empty, `find_or_push` returns the existing index.

**Step 4b: `StringView`** (`test_string_view`)
- [x] Equality, including different lengths with a shared prefix.

**Step 4c: Lexer** (`test_lexer`)
- [ ] Regression tests from bugs already found: `" x"`, `"x\n"`, a file that is just `+`, `==;`, `''`, `'ab'`, `123abc`, 4095- and 4096-char string literals, an empty file, identifier interning (`a b a` → same id for both `a`s).

**Step 5: Commit check (ADR-002)**
- [ ] Enable "Run Tests" in CLion's commit dialog with the CTest configuration.
- [ ] Verify: break a test, try to commit, confirm it's caught.

**Step 6: Sanitizer build**
- [ ] ASan + UBSan build configuration; run the whole suite under it.
- [ ] Fix whatever it finds, and add a regression test for each.

**Step 7: Golden-file tests (ADR-003)**
- [ ] A real `main` that drives the compiler and dumps tokens, ideally behind a flag like `--dump-tokens` (`main.c` is still a sample program).
- [ ] Settle the token-dump format.
- [ ] Runner with pass/fail per file and an "update expected" mode; register with CTest.
- [ ] `.gitattributes` to pin LF line endings for test inputs and expected files.
- [ ] First golden files: every punctuator, every keyword, a small realistic program.

**Step 8: Diagnostics in tests**
- [ ] Once diagnostics are collected (see `TODO.md`), test the error cases: which code, at which position, and that lexing continues after it.

**Step 9: Coverage**
- [ ] Coverage build configuration; find the uncovered lexer branches and test them.

**Later**
- [ ] Parser unit tests and AST golden files.
- [ ] End-to-end tests (compile, link, run, check exit code/stdout), per `educational/followup-setup-testing-expectations.md`.
- [ ] CI (GitHub Actions) as a second safety net.
