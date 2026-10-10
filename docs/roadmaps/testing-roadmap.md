# Testing Roadmap

The step-by-step plan for building the C~~ test suite. The background, structure and decisions behind it are in [
`../testing.md`](../testing.md).

Built step by step. Each step ends in a state that works and can be committed.

**Step 0: Prerequisites**

- [x] A way to create a `SourceText`/`Context` from an in-memory string (`source_text_create_from_buffer` +
  `context_create`).

The real `main` was moved from here to step 7: the harness comes first, so bugs show up one function at a time in unit
tests, not all at once through `main`.

**Step 1: Library split**

- [x] Move everything except `main.c` into a CMake library; `main` links it.
- [x] The compiler still builds and runs exactly as before.

**Step 2: Hand-rolled harness**

- [x] `CHECK` macro (s) and a runner `main` that returns non-zero on any failure.
- [x] One trivial test. Watch it fail (break it on purpose), then pass.

**Step 3: CTest + CLion**

- [x] `enable_testing()` and `add_test` for the test executable.
- [x] Tests appear in CLion's test tree; run them from there.

**Step 4: First real unit tests**, one sub-step per test executable.

**Step 4a: `List`** (`test_list`)

- [x] Push past capacity, pop from empty, `find_or_push` returns the existing index.

**Step 4b: `TextView`** (`test_text_view`)

- [x] Equality, including different lengths with a shared prefix.

**Step 4c: Lexer** (`test_lexer`)

- [x] Single-token and multi-token tests (written by Tomer).
- [x] Regression tests from bugs already found: `" x"`, `"x\n"`, a file that is just `+`, `==;`, `''`, `'ab'`, `123abc`,
  4095- and 4096-char string literals (the old fixed-buffer boundary, now just long literals), an empty file, identifier
  interning (`a b a` → same id for both `a`s), and string interning with `\0` (ADR-010). Written by Claude (marked in
  each test's doc comment). The error cases only check the tokens and recovery; their diagnostics are Step 8.

**Step 5: Commit check (ADR-002)**

- [ ] Enable "Run Tests" in CLion's commit dialog with the CTest configuration.
- [ ] Verify: break a test, try to commit, confirm it's caught.

**Step 6: Sanitizer build**

- [x] ASan + UBSan build configuration (`-DENABLE_SANITIZERS=ON`, see [`../testing.md`](../testing.md), "Sanitizer and
  coverage builds"); the whole suite runs under it.
- [x] The sanitizers are on by default in Debug builds (`ENABLE_SANITIZERS` defaults to ON when `CMAKE_BUILD_TYPE` is
  Debug), so the normal "All CTest" run, and with it the commit check, is sanitized. [ ] Tomer: Reset Cache and Reload
  Project once on the Debug profile, since its cache still says OFF.
- [x] `CMakePresets.json` defines a `coverage` preset (shared, versioned) for CLion; enable it in Settings → Build,
  Execution, Deployment → CMake.
- [ ] Fix whatever it finds, and add a regression test for each. The existing tests are clean. The sanitizer build finds
  the two known `source_text_create_from_path` leaks (file name copied twice, nothing freed for an invalid path) through
  the new `tests/test_source_text.c` (written by Claude): the `source_text` test fails under sanitizers until Tomer
  fixes `SourceText.c`.

**Step 7: Golden-file tests (ADR-003)**

- [ ] A real `main` that drives the compiler and dumps tokens, ideally behind a flag like `--dump-tokens` (`main.c` is
  still a sample program).
- [ ] Settle the token-dump format.
- [ ] Runner with pass/fail per file and an "update expected" mode; register with CTest.
- [ ] Extend `.gitattributes` (it already pins LF for `*.sh`) to the test inputs and expected files.
- [ ] First golden files: every punctuator, every keyword, a small realistic program.

**Step 8: Diagnostics in tests**

- [ ] Once diagnostics are collected (see `TODO.md`), test the error cases: which code, at which position, and that
  lexing continues after it. Start from the Step 4c error cases (`''`, `'ab'`, `123abc`), which today check only the
  tokens.

**Step 9: Coverage**

- [x] Coverage build configuration (`-DENABLE_COVERAGE=ON`) and `tests/coverage.sh`, which lists the lines that never
  ran and the branches never taken.
- [x] Find the uncovered lexer branches and test them: `Lexer.c` went from 86.8% of lines and 76.8% of branches taken to
  100% of lines and 92.0% of branches (11 tests added to `test_lexer.c`, written by Claude). What remains is `lexer_pop`
  's `peek != LEXER_EOF` check (`Lexer.c:77`): every caller peeks first, so it can't be reached through the public
  interface.
- [x] Coverage runs with the rest of the suite: the CTest test `coverage` (so also in "All CTest" and the commit check)
  fails if a line of `Lexer.c` never ran. When it fails, the report in the test output lists the lines; add a test for
  them (or, for a new feature, the test comes with it).
- [ ] After the lexer changes (comments, diagnostics), look at the never-taken branches in the report and cover the new
  ones.

**Step 10: Lexer milestone**

- [ ] Claude reminds Tomer that the roadmap is finished. With his approval, tag the commit:
  `git tag -a lexer-v1 -m "Lexer complete for v1 scope"`.

**Later**

- Parser tests (unit tests, AST dump, golden files) are part of [`parser-roadmap.md`](parser-roadmap.md), from its first
  step.
- [ ] End-to-end tests (compile, link, run, check exit code/stdout), per [
  `../educational/followup-setup-testing-expectations.md`](../educational/followup-setup-testing-expectations.md).
- [ ] CI (GitHub Actions) as a second safety net.
