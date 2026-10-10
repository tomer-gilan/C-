# Docs Index

Every document under `code/docs/`, what it's for, and whether it's binding.

- **Binding:** the source of truth. Code and advice must agree with it; if they don't, one of them gets fixed.
- **Working:** living notes and plans (TODOs, research questions). Current, but not a spec.
- **Reference:** lookup material about C and compilers in general, not about decisions in this project.
- **Educational:** under `educational/`. Background reading, old reviews and early notes. Non-binding and may be out of date.

| File | Kind | What it's for |
|---|---|---|
| [SUBSET.md](SUBSET.md) | Binding | The C~~ language subset, by Annex A section. Anything not listed is out of scope. |
| [overview.md](overview.md) | Binding | High-level plan: the pipeline and key project decisions. (Needs a refresh, see `TODO.md`.) |
| [architecture-decisions.md](architecture-decisions.md) | Binding | ADR log: significant design decisions, why they were made, and their consequences. |
| [implementation-defined.md](implementation-defined.md) | Binding | C~~'s documented choices for implementation-defined behavior (C11 Annex J.3): type sizes, char signedness, diagnostics format, escapes, `>>`, UB policy, and so on. |
| [conventions.md](conventions.md) | Binding | Coding conventions for the compiler's source: naming, objects, `int` vs `size_t`, deep const, macros, errors, ownership, Doxygen. |
| [testing.md](testing.md) | Binding | How the test suite is structured, and the step-by-step roadmap for building it. |
| [TODO.md](TODO.md) | Working | Open tasks: diagnostics, allocation wrappers, docs to write, and so on. |
| [Handoff.md](Handoff.md) | Working | Current session state for Claude, rewritten on "handoff" (see `CLAUDE.md`). Git-ignored: exists only on this machine. |
| [research.md](research.md) | Working | Topics to study, as lists of questions (translation phases, source slices, Valgrind, sanitizers, coverage). |
| [reviews/2026-09-30-code-review.md](reviews/2026-09-30-code-review.md) | Working | Second code review (lexer and whole codebase): build break, new lexer bugs found by running it, status of the first review's findings, and regression inputs. |
| [annex-a-breakdown.md](annex-a-breakdown.md) | Reference | Walkthrough of the standard's Annex A (the C grammar), section by section. |
| [dictionary.md](dictionary.md) | Reference | Glossary of compiler and C terminology, plus an operator precedence table. |
| [Cheat_Sheets/CLion_Cheat_Sheet.md](Cheat_Sheets/CLion_Cheat_Sheet.md) | Reference | CLion shortcuts (Windows keymap). |
| [Cheat_Sheets/Doxygen_Cheat_Sheet.md](Cheat_Sheets/Doxygen_Cheat_Sheet.md) | Reference | Doxygen comment syntax for C. |
| [educational/translation-limits.md](educational/translation-limits.md) | Educational | The standard's minimum translation limits (§5.2.4.1), and why they're floors, not caps. |
| [educational/followup-setup-testing-expectations.md](educational/followup-setup-testing-expectations.md) | Educational | Early advice on setup, I/O without stdio, self-hosting, testing and time estimates. Partly superseded by ADR-001 and `testing.md`. |
| [educational/code-review-lexer.md](educational/code-review-lexer.md) | Educational | Code review of the first lexer (2026-09-27). Paths refer to the old `git_code/` layout. |
| [educational/Lexer.md](educational/Lexer.md) | Educational | Early stub: token categories and character classes. |
