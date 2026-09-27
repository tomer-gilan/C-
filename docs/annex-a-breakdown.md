# Annex A — Section by Section Breakdown

---

## A.1 — Lexical Grammar
> What the **lexer** deals with. Raw characters → tokens.

---

### A.1.1 Lexical elements
Two token flavors exist: **preprocessing-tokens** (for the preprocessor, looser rules) and **tokens** (for the compiler proper, stricter). Your lexer produces the latter.  
Example: `123abc` is a valid preprocessing-token but not a valid token.

---

### A.1.2 Keywords
Reserved words that can never be used as identifiers — the lexer must recognize them before trying to classify anything as an identifier.  
Example: `int` → `KW_INT` token, not `IDENT("int")`.  
**Gotcha:** C11 added `_Alignas`, `_Atomic`, `_Generic`, `_Noreturn`, `_Static_assert`, `_Thread_local`. They start with `_` + uppercase so they don't collide with existing code. For your subset, most of these are irrelevant.

---

### A.1.3 Identifiers
A name starting with a letter or `_`, followed by any number of letters, digits, or underscores.  
Example: `my_var`, `_count`, `X123`.  
**Gotcha:** digits cannot start an identifier — `3x` is not `IDENT("3x")`, it starts as the integer `3` and then `x` is a separate token.

---

### A.1.4 Universal character names
A way to embed Unicode characters in source code using `\uXXXX` or `\UXXXXXXXX`.  
Example: `é` represents `é`.  
**Skip entirely for your subset.**

---

### A.1.5 Constants
Numeric and character literals, in multiple bases and formats.  
Examples:
- `42` — decimal integer
- `052` — octal (leading `0`)
- `0x2A` — hexadecimal
- `3.14f` — float
- `'a'` — character constant

**Gotcha:** `0` alone is an octal constant, not decimal — though the value is the same.  
**Gotcha:** `42u`, `42L`, `42ULL` — suffixes change the type. For your subset, support plain decimal integers and `'x'` character constants. Skip octal, hex, float, and suffixes until needed.

---

### A.1.6 String literals
A sequence of characters between double quotes, stored as a null-terminated `char` array.  
Example: `"hello"` → `{'h','e','l','l','o','\0'}`.  
**Gotcha:** adjacent string literals are concatenated by the preprocessor: `"foo" "bar"` → `"foobar"`. This happens before your compiler sees them.  
**Gotcha:** `L"hello"` is a wide string literal (each char is `wchar_t`). Ignore wide/unicode prefixes for your subset.

---

### A.1.7 Punctuators
All operator symbols and structural delimiters — every non-alphanumeric token.  
Example: `[`, `{`, `++`, `->`, `<<=`, `...`.  
**Gotcha:** `<:` `:>` `<%` `%>` are **digraphs** — alternative spellings for `[` `]` `{` `}`. Technically valid C. Ignore them for your subset unless you want a headache.

---

### A.1.8 Header names
The `<stdio.h>` or `"myfile.h"` forms — only valid as tokens immediately after `#include`.  
Example: `#include <stdio.h>` — `<stdio.h>` is a header-name token, not a comparison.  
**Skip for your subset** (no preprocessor = no header names).

---

### A.1.9 Preprocessing numbers
Numbers as the preprocessor sees them — more permissive than actual numeric constants, before they're validated.  
Example: `1e+foo` is a valid pp-number even though it's not a valid float.  
**Skip for your subset.**

---

## A.2 — Phrase Structure Grammar
> What the **parser** deals with. Tokens → AST.

---

### A.2.1 Expressions
The full expression grammar, from atomic values up to assignments. **The hierarchy encodes operator precedence** — lower in the grammar tree = tighter binding.

The levels from tightest to loosest:

| Rule | Operators | Example |
|---|---|---|
| primary-expression | literals, identifiers, `(expr)` | `42`, `x`, `(a+b)` |
| postfix-expression | `[]`, `()`, `.`, `->`, `++`, `--` | `arr[i]`, `foo()`, `p->x`, `x++` |
| unary-expression | `++`, `--`, `&`, `*`, `+`, `-`, `~`, `!`, `sizeof` | `*p`, `&x`, `!flag`, `sizeof(int)` |
| cast-expression | `(type)` | `(int)3.14` |
| multiplicative | `*`, `/`, `%` | `a * b` |
| additive | `+`, `-` | `a + b` |
| shift | `<<`, `>>` | `x << 2` |
| relational | `<`, `>`, `<=`, `>=` | `a < b` |
| equality | `==`, `!=` | `a == b` |
| AND | `&` | `a & b` |
| exclusive-OR | `^` | `a ^ b` |
| inclusive-OR | `\|` | `a \| b` |
| logical-AND | `&&` | `a && b` |
| logical-OR | `\|\|` | `a \|\| b` |
| conditional | `?:` | `a ? b : c` |
| assignment | `=`, `+=`, `-=`, etc. | `x = 5` |
| expression (comma) | `,` | `a=1, b=2` |

**Gotcha:** The comma operator `,` as an expression (`a=1, b=2`) is the lowest-precedence operator and evaluates both sides but returns the right side. It's distinct from the comma in function arguments. Most compilers rarely use it — safe to skip.  
**Gotcha:** `constant-expression` is just a conditional-expression that must be evaluable at compile time. Used in array sizes (`int a[10]`), enum values, and `_Static_assert`.  
**Gotcha:** `_Generic` (generic-selection) is a C11 type-based dispatch macro. Ignore it.

---

### A.2.2 Declarations
How variables, types, and functions are declared. Structure: `specifiers declarator = initializer;`

Key sub-rules:

**declaration-specifiers** — the type and modifiers: `static const int`, `unsigned long`.  
**storage-class-specifier** — `extern`, `static`, `typedef`, `auto`, `register`. For your subset: ignore all except possibly `extern` later.  
**type-specifier** — the actual type: `int`, `char`, `void`, `struct X`, `enum Y`. This is where type selection lives.  
**type-qualifier** — `const`, `volatile`, `restrict`. Modifiers on the type. Skip for your subset.  
**declarator** — combines pointer stars with the variable name: `*p`, `**pp`, `arr[10]`, `fn(int, int)`.  
**pointer** — the `*` in a declarator, can chain: `int **p` = pointer to pointer to int.  
**parameter-type-list** — function parameters. `...` at the end = variadic (skip for your subset).  
**initializer** — the value after `=`: either an expression (`= 5`) or a brace-enclosed aggregate (`= {1, 2, 3}`).  
**designation** — named initializers in aggregates: `.x = 5`, `[2] = 7`. Skip for your subset.  
**static_assert-declaration** — `_Static_assert(sizeof(int)==4, "need 32-bit int")`. Compile-time assertion. Skip.

**Gotcha:** `int *a, b;` — `b` is `int`, not `int*`. The `*` belongs to the declarator, not the type specifier. This trips up everyone.

---

### A.2.3 Statements
The six categories of statements that make up function bodies.

**labeled-statement** — a target for `goto` (`foo:`), or `case`/`default` inside `switch`. Skip `goto` and `switch` for your subset.  
**compound-statement** — a `{ }` block. Can contain both declarations and statements mixed (C99+). Example: `{ int x = 5; x++; }`.  
**expression-statement** — any expression followed by `;`. The most common statement. A lone `;` is a null statement.  
**selection-statement** — `if`/`else` and `switch`. For your subset: `if`/`else` only.  
**iteration-statement** — `while`, `do-while`, `for`. For your subset: `while` minimum; `for` is sugar for `while`.  
**jump-statement** — `goto`, `continue`, `break`, `return`. For your subset: `return` is required; `continue`/`break` are useful inside loops; `goto` skip.

**Gotcha:** The **dangling else** — `if (a) if (b) x; else y;` is ambiguous. The standard resolves it in prose: `else` binds to the nearest `if`. Your parser must implement this explicitly.  
**Gotcha:** `for (declaration; expr; expr)` — C99 allows declaring a variable inside the `for` init. Treat it as a separate case from `for (expr; expr; expr)`.

---

### A.2.4 External definitions
The top-level structure of a C file.

**translation-unit** — the whole file: a sequence of external declarations.  
**external-declaration** — either a function definition or a top-level variable declaration.  
**function-definition** — `int foo(int x) { ... }` — specifiers + declarator + optional old-style param list + body.  
**declaration-list** — K&R style parameter declarations (pre-ANSI C). Example: `int foo(x) int x; { ... }`. **Ignore entirely** — it's ancient and you'll never encounter it in practice.

**Gotcha:** A declaration at the top level (outside any function) is an external-declaration, not a statement. Your parser's entry point is `translation-unit`, not `statement`.

---

## A.3 — Preprocessing Directives
> The preprocessor's own grammar — runs before the compiler sees anything.

Each line starting with `#` is a directive. Key ones:

| Directive | Purpose |
|---|---|
| `#include` | Paste another file's contents here |
| `#define` | Define a macro (text substitution) |
| `#undef` | Remove a macro |
| `#if` / `#ifdef` / `#ifndef` / `#elif` / `#else` / `#endif` | Conditional compilation |
| `#error` | Emit a compile error with a message |
| `#pragma` | Implementation-defined behavior hints |

**For your subset:** The preprocessor is a separate, independent stage. You can skip it entirely and require that input files have no `#` directives. Add it later as an optional first pass if you want.

**Gotcha:** `#define` can be a simple replacement (`#define PI 3.14`) or a function-like macro (`#define MAX(a,b) ((a)>(b)?(a):(b))`). The two forms have different BNF rules. Function-like macros are the source of most macro-related bugs in real C code.
