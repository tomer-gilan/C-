# C Compiler — Follow-up: Setup, Testing, Expectations

---

## Dev Environment — Skip the VM, Use WSL2

The best setup for "develop on Windows, run on Linux" is **WSL2** (Windows Subsystem for Linux 2), not a VM. WSL2 runs a real Linux kernel inside a lightweight VM that Windows manages transparently. You don't deal with VM overhead, networking, or shared folders — it just works.

What you get:
- A full Ubuntu (or Debian, Arch, etc.) environment accessible from Windows Terminal
- CLion has native **WSL2 toolchain support**: you point it at the WSL2 GCC/clang, it compiles and runs/debugs inside Linux automatically, with the editor running in Windows
- VS Code also has excellent WSL2 integration via the Remote-WSL extension
- `\\wsl$\Ubuntu\...` lets Windows Explorer browse your Linux filesystem directly
- Performance is near-native for CPU-bound work like compiling

Setup is: enable WSL2 in Windows features → install Ubuntu from the Microsoft Store → in CLion, add a WSL toolchain under Settings → Build → Toolchains.

The only thing that's different from a bare Linux machine: you can't easily work with raw hardware, but that's irrelevant for a compiler project.

---

## I/O Without stdio.h

You don't need the preprocessor for I/O, and you don't necessarily need libc either.

**Option 1 — libc without headers (recommended for early stages)**
`printf`, `puts`, `write`, `read`, `malloc`, `free` are all regular functions in libc. You can call any of them without `#include` as long as you provide a forward declaration manually in your source. So your test programs can just declare:

```c
int puts(const char *s);
int printf(const char *fmt, ...);
```

at the top of the file, without ever touching the preprocessor. The linker finds the symbols in libc.

**Option 2 — Raw Linux syscalls (most standalone)**
The Linux kernel exposes everything you need through syscalls. The two essential ones for I/O are:

- `write(fd, buf, len)` — fd=1 is stdout, fd=2 is stderr
- `read(fd, buf, len)` — fd=0 is stdin
- `exit(code)` — terminate the process

You can invoke these via the `syscall` instruction directly from assembly, or by calling the libc wrappers (which are just thin wrappers around the instruction anyway). If you want to go fully libc-free, your compiler generates a `_start` label instead of `main`, calls syscalls directly, and you link with `ld` instead of `cc` (which wouldn't pull in libc startup code).

For early development, Option 1 is fine. Going fully libc-free is a meaningful milestone but adds complexity — `printf`-style formatting is something you'd have to implement yourself.

**Bottom line on the preprocessor:** it is genuinely independent of the rest of the compiler. You can build a complete, working compiler that simply doesn't accept `#include` or `#define` in its input, and all I/O still works via Option 1 or 2. Add the preprocessor as a separate, optional first pass later.

---

## Self-Hosting Path

The classic path looks like this:

**Phase 1 — Bootstrap compiler (CC1)**
Write your compiler in full, unconstrained C. Compile it with GCC. It targets your defined subset. At this point you have a working compiler but it's compiled by GCC.

**Phase 2 — Compile your compiler with itself**
Your compiler's source code must itself only use features in your subset. So you gradually rewrite the compiler source to avoid anything outside the subset (no `struct` until you implement struct, no `for` until you implement `for`, etc.). When your compiler can compile its own source and produce an identical binary, you've self-hosted.

**Phase 3 — Drop GCC**
With a self-hosting compiler, GCC is no longer in the critical path. You compile CC1 with CC1.

**Suggestion on order of operations**
Don't constrain yourself from the start. Write the compiler in whatever C is comfortable. Once you have a working compiler, *then* audit what features of C your own source actually uses — you'll find the list is shorter than expected (compilers tend to use very basic constructs). Define the subset to be at least a superset of what your compiler actually needs, and you'll reach self-hosting with less rewriting.

One constraint worth noting early: if you want to self-host, your compiler needs to handle at minimum: `struct`, `char*` strings, arrays, function pointers (maybe), and possibly `typedef`. These are required by almost any non-trivial C program. Plan your subset accordingly.

---

## Testing — What to Know as a Beginner

Compiler testing is actually well-suited for someone new to testing, because the test contract is extremely clear: given this input, produce this output or this error.

**The structure of a compiler test suite**

Three categories:

1. **Valid programs with expected output** — programs in your subset that should compile successfully, run, and produce a specific exit code or stdout. Most of your tests will be this.
2. **Invalid programs with expected errors** — programs that should fail to compile, ideally with a specific error message. Important for catching that your error handling works.
3. **Edge cases** — things that are legal but subtle: integer overflow behavior, pointer arithmetic, function calls with many arguments, etc.

**The minimal test runner**

Don't use a framework to start. A shell script like this does the job:

```
tests/
  001_return_42.c          # expected exit code: 42
  002_addition.c           # expected exit code: 5
  002_addition.expected    # contains "5" (stdout check)
  run_tests.sh             # compiles each .c, runs it, compares
```

`run_tests.sh` compiles each test file with your compiler, runs the binary, checks `$?` (exit code) or stdout against expected, and prints PASS/FAIL. 20 lines of shell.

**GitHub Actions**

Add a `.github/workflows/test.yml` that runs `run_tests.sh` on every push. The YAML is trivial — install gcc (to compile your compiler's source), build your compiler, run tests. GitHub shows green/red on each commit. This is the entire CI setup you need for a long time.

**CLion integration**

CLion supports CTest (CMake's test runner) natively — it shows tests in a tree with green/red status. But for a compiler project, wiring it up is more effort than it's worth early on. Use the shell script for months, add CTest later if you want the IDE integration.

**The testing discipline that matters most**

Write a test *before* you implement each feature, verify it fails, then implement the feature, then verify it passes. This sounds obvious but it's the only way to know your test actually tests something. Every time you fix a bug, add a regression test for that bug before fixing it.

---

## Difficulty and Time Expectations

Your JPEG codec background is genuinely useful context. That project required understanding a multi-stage pipeline (color transform → block partitioning → DCT → quantization → entropy coding), managing state across stages, and debugging where the data flowing between stages went wrong. A compiler has the same character — the stages are different but the pattern of "something goes wrong somewhere in the pipeline and you have to figure out where" is identical.

**What's similar to the JPEG project**
- Multi-stage pipeline with data flowing between stages
- A formal specification you need to interpret correctly
- Debugging requires understanding what the output "should" look like at each stage
- There's a natural build order (you can't test code gen until the parser works)

**What's different**
- More system-level: you'll be reading x86-64 instruction references, ABI documents, and ELF specs
- The output (assembly) is human-readable and debuggable, which helps
- Formal language theory (grammars, parsers) is a new domain if you haven't studied it
- Linux toolchain fluency is required — objdump, gdb, nm, strace

**Honest difficulty assessment**

The easy parts: lexer, basic expression parsing, simple code generation for arithmetic. These go quickly and feel rewarding.

The hard parts, roughly in order:
1. **Expression parsing with correct operator precedence** — the grammar is tricky. Use Pratt parsing or precedence climbing.
2. **The calling convention** — this will consume more time than you expect. Stack alignment, register assignment, callee/caller-save registers, passing structs (eventually). Get it wrong and you get silent memory corruption.
3. **The type system + pointer arithmetic** — `int*` + 1 advances by 4 bytes, not 1. Array-to-pointer decay. When is something an lvalue vs. rvalue. This interacts with code gen in subtle ways.
4. **Self-hosting** — finding and fixing all the things your compiler does slightly wrong that GCC was silently tolerating.

**Time estimate**

Assuming roughly 8-10 hours/week, with your background and learning Linux from scratch:

| Milestone | Estimate |
|---|---|
| Toolchain set up, Linux basics, first generated assembly that runs | 2-3 weeks |
| Working lexer + parser for expressions | 3-5 weeks |
| Variables, if/while, basic functions | 4-6 weeks |
| Pointers + arrays | 3-4 weeks |
| Heap (malloc/free as library calls) | 1-2 weeks |
| Solid test suite, error reporting | ongoing |
| **Working compiler for the subset** | **~4-6 months** |
| Compiler compiles itself (self-hosting) | +3-5 months |

These are rough — some people go faster, especially if they've done formal language theory before. Some people spend a month on calling conventions alone.

**The psychological pattern to expect**

The first two weeks feel slow (setup, unfamiliar territory). Weeks 3-8 feel fast (the lexer and parser are intellectually satisfying and relatively clean). Then code generation starts and it slows down again because debugging assembly is a skill you have to develop. Around the point where you get your first real program to compile correctly, there's a strong motivational spike. Use that to push through the harder parts.

---

## One Thing You Didn't Mention

Your point 3 in the last message was blank — you started a "3." and stopped. Let me know what you meant to write there.
