# Coding Conventions

How the C~~ compiler's own source is written. These rules were inferred from the existing code and confirmed on 2026-09-30. New code follows them; code that doesn't is either a bug or a reason to change this file first.

---

## Naming

- **Types:** `PascalCase` (`SourceFile`, `TokenKind`, `FilePosition`).
- **Functions:** `module_verb[_object]` in snake_case, where the prefix is the module or type name (`list_find_or_push`, `file_position_get_source_file`, `lexer_lex_source_file`).
- **Enum constants:** `TYPENAME_VALUE` in UPPER_SNAKE (`TOKEN_KW_INT`, `DIAGNOSTIC_SEVERITY_ERROR`).
- **Sentinels and aliases** use a double underscore after the prefix: `TOKEN__SPELLABLE_COUNT`, `DIAGNOSTIC_CODE__COUNT`, `TOKEN__KW_FIRST`.
- **Files:** one module per `PascalCase.c` / `PascalCase.h` pair. Include guards: `C_COMPILER_<NAME>_H`.

## Modules and objects

- The object parameter is named **`this`** and comes first.
- Heap objects come in **`X_create` / `X_destroy`** pairs. `create` returns a handle, or `NULL` when the failure is the *user's* fault (e.g. a bad path). A broken precondition is an assert, not `NULL`.
- A struct is **opaque** (declared in the header, defined in the `.c`) when it has private invariants: `Lexer`, `List`.
- **Plain-data** structs are public and passed by value: `Token`, `Range`, `FilePosition`, `StringView`.

## Integer types

- Sizes, lengths, offsets and indices are **`int`**, not `size_t`. This is deliberate:
  - It avoids signed/unsigned comparison bugs.
  - `-1` works as the "not found" sentinel (`list_index_of`).
  - It stays inside the C~~ subset (no `unsigned`, no `size_t`), keeping self-hosting possible (ADR-004).

## `const`

- **Deep const:** a pointer parameter is `const` only if calling the function can't change the observable state of the object, *including state reached through its fields*. Example: `file_position_get_source_file` takes a non-`const Context*`, because the `SourceFile*` it returns can be used to modify the context's state.
- Pointee `const` (`const List* this`) appears in both declarations and definitions.
- Top-level `const` on value parameters (`const int offset`) appears in **definitions only**. It's legal to leave it out of the header because top-level const isn't part of the function's type.

## Macros

- **Function-like macros are banned**, except where only a macro can do the job. Each exception must justify itself in its doc comment. Current exceptions:
  - `string_view_from_literal` (StringView.h): needs `sizeof` of a string literal at compile time.
  - The test harness's `CHECK` (planned, `testing.md`): needs `__FILE__`, `__LINE__` and `#cond`.
- **Object-like constants** (`#define LEXER_EOF ...`) are tolerated, but `enum` or `static const` is preferred.

## Errors

The full policy and its rationale are in **ADR-011** (`architecture-decisions.md`). In short:
- `assert(cond && "message")` is only for compiler bugs and broken caller preconditions.
- Anything wrong in the *user's source* becomes a diagnostic, and processing continues.
- Library code never calls `exit`. Only the driver decides to stop, after a stage that reported errors. The one exception is allocation failure (ADR-013).

## Ownership

- By default a pointer parameter is **borrowed**: the caller keeps ownership.
- Every **transfer** is documented with `@warning ownership of ... is transferred to the caller/callee` (see `List.h`).
- Containers **own their elements** and take a `destroy_element` callback (`list_create(free)`).
- **`*_list_*` adapters** (`string_view_list_equals`, `source_file_list_destroy`, `utils_string_list_strcmp`) wrap typed functions in the `void*` signatures `List` expects. Mark them with a `// for list.h` comment.

## Documentation

- Doxygen syntax, per [Cheat_Sheets/Doxygen_Cheat_Sheet.md](Cheat_Sheets/Doxygen_Cheat_Sheet.md).
- Every public function gets a `/** */` block in its header: `@param`, `@return` / `@retval`, and `@warning` / `@attention` / `@note` for contracts.
- `static` functions get the same treatment in their `.c` file.
- Struct members: `///<` trailing comments.
- Inline: code names in `@c`, parameter names in `@b`.
