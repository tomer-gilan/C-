# AST Dump Format

What `AstSerializer` prints for each node. Parser tests compare against the plain dump, and sema tests against the typed
dump (parser roadmap Step 2, sema roadmap). Written 2026-10-28.

`AstSerializer` lives in `tests/AstSerializer.c/.h` (the CMake library `test_ast_serializer`): it's a test tool, not
part of the compiler. Since 2026-10-31 it is Claude's to maintain, like the tests (Tomer's decision), so a new AST node
kind needs a case there before its dump tests can pass.

## General rules

1. **Only nodes print** (ADR-026). Wrappers (`…Item`) are invisible: a list prints its elements, in order. The one
   exception is `DeclaratorItem` with an initializer, which prints as `init-declarator` (see Declarations).
2. **A node is `(head children…)`.** Leaves are atoms: a constant prints its value, an identifier its spelling.
3. **Every node kind has a fixed shape.** Optional children are listed in the table. No child ever prints as an empty
   placeholder, except `(parameters)` for a function with no parameters.
4. **Parentheses produce nothing.** `((2))` prints `2`.
5. **The number of operands tells same-spelled operators apart:** `(- 2)` is unary minus, `(- 1 2)` is subtraction.
   The same goes for `*` (dereference or multiply) and `&` (address-of or bitwise and).
6. **No ranges and no ids.** Dedicated tests check ranges.
7. **Atoms never contain spaces.** That includes type spellings and constants.

### Notation

| Symbol | Meaning |
|---|---|
| `⟦X⟧` | the dump of X |
| `x*` | zero or more `x`, space-separated |
| `x+` | one or more `x`, space-separated |
| `x?` | optional |
| `T` | a type spelling (see Atoms) |
| `name` | an identifier's spelling |

## Atoms

| Thing | Formal | C | Dump |
|---|---|---|---|
| Identifier (expression) | `name` | `x` | `x` |
| Integer constant | value in decimal | `0x10` | `16` |
| Character constant | C spelling in `'…'`, with escapes | `'\n'` | `'\n'` |
| String literal | C spelling in `"…"`, with escapes | `"hi\n"` | `"hi\n"` |
| Type `T` | base type, then one suffix per type constructor, innermost first: `*` (pointer), `[N]` (array), `(T,T,…)` (function, `(void)` with no parameters). No spaces | `char *a[3]`, `int f(int, char *)` | `char*[3]`, `int(int,char*)` |

**Types read right to left**, from the outermost constructor in: `char*[3]` is an array of 3 pointers to `char`.
This is not C's spelling for every type:

| C | Dump | Read right to left |
|---|---|---|
| `int a[2][3]` | `int[3][2]` | array of 2 arrays of 3 `int` |
| `int *p[3]` | `int*[3]` | array of 3 pointers to `int` |
| `&a` with `int a[3]` | `int[3]*` | pointer to an array of 3 `int` (C: `int(*)[3]`) |
| `char *f(void)` | `char*(void)` | function with no parameters returning a pointer to `char` |
| `f` with `int f(int)`, decayed | `int(int)*` | pointer to a function taking an `int` returning `int` (C: `int(*)(int)`) |

Every constructor is a suffix, so the dump never needs parentheses, and the serializer prints a type with one
post-order recursion. **Gotcha:** multi-dimensional arrays come out reversed compared with C (`int[3][2]` for
`int a[2][3]`), so write test expectations from the type, not from the declaration.

## External definitions

| Node | Formal | C | Dump |
|---|---|---|---|
| `TranslationUnitNode` | `(translation-unit ⟦item⟧*)` | `int g; int main(void) { return g; }` | `(translation-unit (declaration g:int) (function-definition main:int(void) (parameters) (block (return g))))` |
| `ExternalDeclarationItem` (wrapper) | invisible: `⟦its function definition or declaration⟧` | — | — |
| `FunctionDefinitionNode` | `(function-definition ⟦declarator⟧ (parameters ⟦item⟧*) ⟦body⟧)` | `int add(int a, int b) { return a + b; }` | `(function-definition add:int(int,int) (parameters a:int b:int) (block (return (+ a b))))` |

## Declarations

| Node | Formal | C | Dump |
|---|---|---|---|
| `DeclaratorNode` | `name:T`. An unnamed parameter prints as `:T` | `int *p` | `p:int*` |
| `DeclaratorItem` (wrapper) | without an initializer: `⟦declarator⟧`. With one: `(init-declarator ⟦declarator⟧ ⟦initializer⟧)` | `x = 2` | `(init-declarator x:int 2)` |
| `DeclarationNode` | `(declaration ⟦item⟧+)` | `int a, *p = 0;` | `(declaration a:int (init-declarator p:int* 0))` |
| Prototype (a `DeclarationNode`) | same as above. Parameter names don't print, since they aren't part of `T` | `int putchar(int c);` | `(declaration putchar:int(int))` |

`init-declarator` is the standard's grammar name for a declarator plus its initializer. It is the only form that a
wrapper prints.

## Statements

| Node | Formal | C | Dump |
|---|---|---|---|
| Compound | `(block ⟦item⟧*)` | `{ int x; x = 1; }` | `(block (declaration x:int) (= x 1))` |
| `BlockItem` (wrapper) | invisible: `⟦its statement or declaration⟧` | — | — |
| Return | `(return ⟦expression⟧?)` | `return;` | `(return)` |
| Expression statement | `⟦expression⟧` | `f(1);` | `(call f 1)` |
| Null statement | `(null)` | `;` | `(null)` |
| If | `(if ⟦condition⟧ ⟦then⟧ ⟦else⟧?)` | `if (x) return 1; else ;` | `(if x (return 1) (null))` |
| While | `(while ⟦condition⟧ ⟦body⟧)` | `while (n) n = n - 1;` | `(while n (= n (- n 1)))` |

An expression statement prints its bare expression. It can't be mistaken for a statement, because every statement's
head is a keyword (`block`, `return`, `null`, `if`, `while`, `declaration`), and no expression's head is.

## Expressions

| Node | Formal | C | Dump |
|---|---|---|---|
| Parenthesized | `⟦expression⟧` (no node) | `((2))` | `2` |
| Unary `- + ! ~ * &` | `(op ⟦operand⟧)` | `-*p` | `(- (* p))` |
| `sizeof` | `(sizeof ⟦operand⟧)` | `sizeof a[0]` | `(sizeof ([] a 0))` |
| Binary `* / % + - << >> < > <= >= == != & ^ \| && \|\|` | `(op ⟦left⟧ ⟦right⟧)` | `1 + 2 * 3` | `(+ 1 (* 2 3))` |
| Assignment | `(= ⟦left⟧ ⟦right⟧)` | `a = b = 0` | `(= a (= b 0))` |
| Subscript | `([] ⟦left⟧ ⟦right⟧)`, in the order written | `i[a]` | `([] i a)` |
| Call | `(call ⟦callee⟧ ⟦argument⟧*)` | `f()` | `(call f)` |

## Typed mode

Sema never writes into the AST (ADR-023), so expression types come from sema's side table: `ast_serializer_create`
takes `SemaResult`'s `const ExpressionSemantics*` (indexed by expression id), `NULL` for the plain dump. The serializer
prints each type itself, with the spelling under Atoms. This makes the serializer depend on `Sema.h`, which is
accepted: it's an internal test tool, and this is the simplest way to produce the dump (Tomer, 2026-10-31).

- Every expression's head or atom gets `:T`, where `T` is the type sema recorded (after decay, ADR-024):
  `1 + 2 * 3` dumps as `(+:int 1:int (*:int 2:int 3:int))`, and `f(1)` as `(call:int f:int(int) 1:int)`.
- `int a[3];` then `a + 1` dumps as `(+:int* a:int* 1:int)`, but `sizeof a` dumps as `(sizeof:int a:int[3])`.
- Declarators already carry `:T`, so they print the same in both modes. A parenthesized expression has no node, so it
  gets no type.
- The table is passed to `create`, for the whole dump, so a whole program can be dumped typed.

**Gotcha:** in typed mode, a declarator `y:int` and an identifier expression `y:int` look the same. Only their position
tells them apart: a declarator only appears directly under `declaration`, `parameters`, `init-declarator` or
`function-definition`.

## Whitespace: FLAT and NESTED

The two modes print the same tokens and differ only in whitespace.

- **FLAT:** one line, single spaces, no trailing newline. Used for unit-test strings.
- **NESTED:** each function definition, declaration and statement starts a new line, indented 2 spaces deeper than
  its parent. Everything else stays on its parent's line: expressions, atoms, `(parameters …)`, and the condition of an
  `if` or `while`. Closing parentheses go at the end of the last child's line. The dump ends with one newline.

Example:

```c
int add(int a, int b) { return a + b; }
int main(void) { int x = 2, y; if (x) y = add(x, -3); return y; }
```

```
(translation-unit
  (function-definition add:int(int,int) (parameters a:int b:int)
    (block
      (return (+ a b))))
  (function-definition main:int(void) (parameters)
    (block
      (declaration (init-declarator x:int 2) y:int)
      (if x
        (= y (call add x (- 3))))
      (return y))))
```

The same program in FLAT:

```
(translation-unit (function-definition add:int(int,int) (parameters a:int b:int) (block (return (+ a b)))) (function-definition main:int(void) (parameters) (block (declaration (init-declarator x:int 2) y:int) (if x (= y (call add x (- 3)))) (return y))))
```
