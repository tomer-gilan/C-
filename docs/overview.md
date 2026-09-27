# Building a C Compiler — Overview

---

## 1. The Compilation Pipeline

The canonical pipeline has these stages, in order:

**Preprocessing** — Before compilation proper begins, a preprocessor handles `#include`, `#define`, `#ifdef`, etc. It's essentially a text-substitution layer that produces a single expanded source file. For your minimal compiler, you can skip this entirely or handle only `#include` trivially.

**Lexing / Tokenization** — The source string is broken into a flat stream of tokens: keywords (`int`, `while`), identifiers, literals, operators, punctuation. This is the easiest stage.

**Parsing** — The token stream is consumed according to a grammar and turned into an Abstract Syntax Tree (AST). This is where the structure of the program becomes explicit. The hardest part here is expression parsing (operator precedence); look up Pratt parsers or recursive descent with precedence climbing.

**Semantic Analysis** — The AST is walked to enforce rules the grammar alone can't catch: type checking, undeclared variable use, function signature mismatches, etc. You annotate the AST with type information here.

**IR Generation** — The annotated AST is lowered into an Intermediate Representation (IR) — a flat, simple instruction set that's easier to reason about than the AST but isn't tied to any specific CPU. Many educational compilers skip a formal IR and go AST → assembly directly; that's fine to start.

**Optimization** (optional) — IR-level passes that simplify, inline, eliminate dead code, etc. Skip entirely for v1.

**Code Generation** — The IR (or AST) is lowered to target assembly. This is the second hardest stage: you must handle register allocation, calling conventions, memory layout, and instruction selection.

**Assembly + Linking** — Don't do this yourself. Emit `.s` assembly text, call `as` (GNU assembler) to produce an object file, and call `ld` or `cc` to link. This saves enormous effort.

---

## 2. Before You Begin — Key Decisions

**Target Architecture**
Pick one and only one. x86-64 is the right choice for a first compiler: it has by far the most documentation, tutorials, reference material, and examples. ARM64 (AArch64) is the alternative if you're on Apple Silicon and want parity with what actually runs natively — but the instruction set is cleaner, which is a trade-off in terms of learning what real compilers deal with.

**Host OS**
Linux is strongly preferred. Reasons: ELF object format is simple and well-documented, the System V AMD64 ABI (the calling convention) is the standard everywhere except Windows, `objdump`/`readelf`/`strace` are invaluable, and most compiler literature assumes it. macOS works but has Mach-O format differences and Apple's linker quirks. Windows is a significant detour — different calling convention (Microsoft x64 ABI), PE/COFF format, and a much more hostile toolchain.

**What to emit**
Emit text assembly (`.s` files). Do not try to emit raw machine code or ELF directly until much later. Emitting assembly lets you read your output, debug it with `gdb`, and skip the entire binary format problem.

**Language to write the compiler in**
This matters more than people admit. C and C++ are traditional (and eventually let you self-host), but managing memory manually while also implementing a compiler is genuinely hard for a first attempt. Rust has excellent data structures for this but a steep learning curve. Python, Go, or even TypeScript let you move fast in the early stages without fighting the language itself. Pick whichever you're most fluent in — the compiler concepts are identical regardless.

**Will you self-host?**
Self-hosting means your compiler can compile itself. It's a meaningful milestone but not required. It does constrain your language choice (your compiler language must be in the subset of C you implement) if you eventually want to go there.

---

## 3. The C Standard

**Where to get it**
The official ISO standard costs money. The freely available committee drafts are nearly identical:
- C99 draft: search for **N1256** (final draft of C99)
- C11 draft: search for **N1570** (the most-referenced free document; this is what you want)
- C17 draft: **N2176** (minor fixes over C11)
- C23 draft: **N3096**

Start with N1570. It's 700 pages but you don't read it cover-to-cover — you use it as a reference when you need to know exactly what the standard says about a specific construct.

**Key concepts in the standard**
- **Undefined Behavior (UB)**: The standard explicitly leaves many things undefined — signed integer overflow, null pointer dereference, reading uninitialized memory, out-of-bounds access. A conforming implementation can do *anything* when UB occurs. You don't need to handle UB correctly, but you need to know it exists so you're not surprised by what real C programs rely on.
- **Implementation-defined behavior**: Things the standard defers to the implementation — `sizeof(int)`, signedness of `char`, etc. Your compiler gets to decide, but must be consistent.
- **Unspecified behavior**: Valid but not guaranteed — e.g., evaluation order of function arguments.

**Common gotchas**
- Integer promotion: short and char operands are silently promoted to int in arithmetic. This trips up type-checking in semantic analysis.
- The `char` signedness question: `char` can be signed or unsigned; pick one and document it.
- `sizeof` returns `size_t` (unsigned), and mixing signed/unsigned comparisons produces subtle bugs in the programs you compile.
- Function declarations without prototypes (K&R style) are legal in older standards and nightmarish to handle.
- Pointer arithmetic is defined only within the same array object. Your type system needs to know this.
- Sequence points: `i++ + i++` is undefined. You don't need to detect it, but don't generate incorrect code assuming left-to-right evaluation.

---

## 4. The Minimal C Subset to Target

This is sometimes called **C0**, **C--**, or informally "baby C." A practical minimal subset that's still useful:

**Types**
`int` and `char` to start. Then `int*` / `char*` (pointer to int, pointer to char). Hold off on `struct`, `union`, `float`/`double`, `long`, `short`, `unsigned` until the basics work. You can add `void` as a return type early since it's simple.

**Variables**
Local variables (stack-allocated) and global variables. No `static` locals yet. No `extern`.

**Functions**
Function definitions with typed parameters and a return value. Function calls. Recursion works for free once you implement the call stack correctly. No variadic functions (`printf` style).

**Control flow**
`if` / `else`, `while`. That's it. `for` is sugar for `while` and can be added trivially. `do-while`, `switch`, `goto`, `break`, `continue` all come later.

**Expressions**
Arithmetic: `+`, `-`, `*`, `/`, `%`. Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`. Logical: `&&`, `||`, `!`. Assignment: `=`. Compound assignment (`+=`, etc.) is sugar and can wait. Ternary (`?:`) is useful but skippable.

**Memory**
Pointer dereference (`*p`), address-of (`&x`), array indexing (`a[i]`, which is sugar for `*(a+i)`). `malloc` and `free` are just function calls — your compiler doesn't need special knowledge of them, they're provided by libc.

**I/O**
`printf` and `scanf` can be treated as regular function calls your compiler doesn't understand the internals of. You link against libc and they just work.

**Explicitly exclude for v1**
`struct`, `union`, `enum`, `typedef`, `switch`, `goto`, `do-while`, `for`, bitfields, VLAs (variable-length arrays), `const`/`volatile`/`restrict` qualifiers, multiple compilation units, the preprocessor, `static`/`extern`, floating point.

This subset is Turing-complete, supports recursion, heap allocation, and can implement non-trivial programs.

---

## 5. General Advice

**Build incrementally, test constantly.** Your first goal should be a compiler that handles one function returning a constant integer and nothing else. Then add addition. Then variables. Then `if`. Each feature should come with a test you can run.

**Emit assembly and read it.** Write a small C program, compile it with `gcc -O0 -S` and read the output. Do this for every language feature before you implement it. You'll know exactly what you're aiming for.

**The calling convention is critical and underestimated.** The System V AMD64 ABI specifies exactly which registers hold which arguments, which registers are caller-saved vs. callee-saved, how the stack must be aligned (16-byte alignment before a `call` instruction), and how return values come back. Get this wrong and everything crashes in confusing ways.

**Study small real compilers.** The best learning resources are small, readable compilers:
- **chibicc** (Rui Ueyama) — built from scratch in ~1 commit per feature, exceptional for learning
- **tcc** (Tiny C Compiler) — a real, production C compiler in a single file
- **8cc** (Rui Ueyama's earlier work) — simpler than chibicc
- **cproc** — a C11 compiler using QBE as a backend

**Books worth having**
- *Engineering a Compiler* (Cooper & Torczon) — the best modern textbook, very readable
- *Modern Compiler Implementation in C* (Appel) — more academic but thorough
- *Crafting Interpreters* (Nystrom) — about interpreters but the lexer/parser/resolver chapters are excellent and free online

**Don't optimize early.** Generating slightly inefficient assembly is fine. Unoptimized `gcc -O0` output is your reference. Match that before thinking about optimization.

**Use `gdb` and `objdump` aggressively.** When your generated program crashes or misbehaves, disassemble it and step through it. This is the core debugging loop.

---

## 6. My Questions for You

These will shape what kind of planning doc is most useful as you go deeper:

1. **What language will you write the compiler in?** This affects everything from data structure design to debugging approach.
2. **Are you on Linux, macOS, or Windows?** This determines which toolchain and ABI apply.
3. **Is self-hosting a goal?** Even a distant one — it changes which subset of C you need to implement.
4. **Do you want to implement a preprocessor**, or will you assume the input is already preprocessed (i.e., no `#include`/`#define`)?
5. **Do you want to use a backend like LLVM or QBE**, or emit x86-64 assembly directly? Using a backend simplifies code generation massively but adds a dependency and hides what's happening.
