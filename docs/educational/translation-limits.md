# Translation Limits (C11 §5.2.4.1)

A list of the minimum limits the C standard requires every conforming implementation to support. **These are floors, not ceilings** — the standard is telling you what a program is guaranteed to be able to rely on across *any* conforming compiler, not capping what an individual compiler is allowed to support. Real compilers (gcc, clang) exceed almost every number here by orders of magnitude, bounded in practice only by available memory. Don't build any of these numbers into your own compiler as an artificial restriction — just don't be surprised if you ever see them cited as "the C standard says X," since that phrasing usually means "the standard guarantees *at least* X," not "X is the maximum."

A strictly portable program, however, *should* stay within these numbers — that's the actual point of the section: it tells a program's author what's safe to assume will work identically on every conforming implementation.

---

## Lexical / preprocessing limits

**4095 characters in a logical source line.**
A single line of source code, after line-splicing (`\` at end of line) is resolved.

**4095 characters in a string literal, after concatenation.**
Applies to the final, merged string — adjacent string literals (`"foo" "bar"` → `"foobar"`,
see [annex-a-breakdown.md](../specifications/annex-a-breakdown.md) A.1.6) are concatenated *before* this limit is measured, not each token
individually.

**63 significant initial characters in an internal identifier or a macro name.**
**31 significant initial characters in an external identifier.**
Beyond this many leading characters, whether two identifiers that share a prefix are treated as the same symbol or different ones is implementation-defined. The external-linkage number is lower for historical reasons — old linkers truncated exported symbol names more aggressively than a compiler's own internal bookkeeping needed to.

**4095 macro identifiers simultaneously defined in one preprocessing translation unit.**

**127 parameters in one macro definition.**
**127 arguments in one macro invocation.**

**63 nesting levels of conditional inclusion** (`#if`/`#ifdef`/`#elif`/... chains, nested).

**15 nesting levels for `#include`d files.**
One file including another including another, 15 deep.

---

## Declaration limits

**12 pointer, array, and function declarators (in any combination) modifying an arithmetic, structure, union, or void type in a declaration.**
Example: `int ****p;` uses 4 of the 12. Stack up enough `*`, `[]`, and `()` on one declarator and you can theoretically hit this — essentially never a concern in practice.

**63 nesting levels of parenthesized declarators within a full declarator.**
Nested parens around a declarator, e.g. in complex function-pointer-returning-function-pointer type spellings
(see [dictionary.md](../specifications/dictionary.md)'s **Function pointer** entry for why these get parenthesis-heavy).

**511 identifiers with block scope declared in one block.**
The number of distinct local variables you can declare inside one `{ }`.

**4095 external identifiers in one translation unit.**
Total distinct globally-linked names (functions + global variables) in one `.c` file.

---

## Expression limits

**63 nesting levels of parenthesized expressions within a full expression.**
Example: `((((((...))))))`, 63 deep, before a conforming implementation is no longer required to handle it.

---

## Function / call limits

**127 parameters in one function definition.**
**127 arguments in one function call.**

---

## Aggregate limits

**1023 members in a single structure or union.**
**1023 enumeration constants in a single enumeration.**
**1023 case labels for a switch statement** (not counting labels belonging to any switch nested inside it).
**63 levels of nested structure or union definitions in a single struct-declaration-list.**

---

## Object size

**65535 bytes in an object, in a hosted environment.**
The minimum guaranteed size for any single object — including, notably, the backing array of a string literal, though the 4095-character string-literal limit above is the binding constraint there since it's comfortably smaller.

**127 nesting levels of blocks.**
`{ { { { ... } } } }`, 127 deep.

---

## Practical takeaway for your compiler

You don't need to enforce any of these as hard caps — that's not what they're for, and doing so would just make your compiler stricter than gcc/clang for no benefit. What's actually useful about this list:

- It tells you the *kinds* of things a fully standards-conforming implementation has to think about (deeply nested parens, huge structs, long identifiers) even if you never hit the numbers in practice with hand-written test programs.
- If you ever write a stress-test/fuzzer for your own lexer or parser, these numbers are a reasonable source of "how
  deep should I nest this to find stack-overflow-style bugs in a recursive-descent parser" — a parser implemented with
  one function call per nesting level (very likely, given [dictionary.md](../specifications/dictionary.md)'s **Recursive descent** entry)
  will blow its own call stack on sufficiently deep nesting long before hitting these standard numbers, which is a real,
  common bug class in hand-written recursive-descent parsers worth being aware of.
