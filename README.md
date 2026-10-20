# PLD-Comp — A C compiler written in C++

`ifcc` is a small compiler for a **subset of the C language**. It reads a single
C source file and emits x86-64 assembly, which is then assembled and linked with
`gcc` to produce a native Linux executable.

The front-end is built with [ANTLR 4](https://www.antlr.org/) (grammar +
generated visitor). The middle-end lowers the syntax tree to a small three-address
intermediate representation (IR) organised as a control-flow graph (CFG) of basic
blocks. The back-end walks that CFG and emits x86-64 assembly.

---

## Project at a glance

| | |
|---|---|
| **What** | A from-scratch compiler for a subset of C (lexer, parser, semantic analysis, IR, code generation) |
| **Language** | C++17 |
| **Tooling** | ANTLR 4 (grammar / parser), GCC, GNU Make, Python (test harness), Doxygen |
| **Target** | x86-64 assembly (System V ABI), assembled and linked by `gcc` |
| **Context** | Team project (hexanôme) for the *PLD-Comp* module, 4IF, INSA Lyon, 2023–2024 |
| **Team size** | 6 students |
| **Duration** | ~5 weeks, iterative sprints |
| **Result** | 142 / 146 functional tests passing |
| **License** | MIT |

### What it does, in one example

Input C (a recursive Fibonacci; note the forward declaration, since this
compiler expects `main` first):

```c
#include <stdio.h>

int fib(int n);

int main() {
    putchar('0' + fib(7) % 10);   /* fib(7) = 13, so print '3' */
    putchar(10);                  /* newline */
    return 0;
}

int fib(int n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n - 2);
}
```

```
$ ./compiler/build/ifcc fib.c > fib.s   # C  -> x86-64 assembly
$ gcc fib.s -o fib && ./fib             # assemble + link + run
3
```

The generated assembly contains the classic function prologue/epilogue, a stack
frame for locals and temporaries, and direct `call` instructions - the same shape
you would get from `gcc -O0`, produced entirely by our own code.

---

## Language support

This is a **teaching compiler**: it deliberately covers a well-defined subset of
C rather than the whole language. The table below reflects what the compiler
actually does today (verified by running the test suite - see *Testing*).

### Supported

- **Types**: `int` (32-bit), `char` (stored as 32 bits, see *Known limitations*),
  `void` return type
- **Declarations**: anywhere inside a block, with optional initialisation,
  comma-separated lists (`int a = 1, b, c;`)
- **Scopes**: nested blocks `{ }`, variable shadowing, per-block symbol tables
- **Expressions**: `+ - * / %`, bitwise `& | ^ ~`, comparisons `== != < <= > >=`,
  logical `!`, unary `-`, parentheses, C precedence and associativity
- **Assignments**: `=`, compound `+= -= *= /= %=`, pre/post `++` and `--`
  (assignment returns a value, as in C)
- **Control flow**: `if` / `else`, `while`, `continue`
- **Functions**: definitions with parameters and an `int`, `char` or `void`
  return type, up to 6 arguments passed in registers per the System V ABI,
  recursion, and forward declarations (`int f(int x);` before `main`)
- **I/O**: `putchar` and `getchar` (wired to libc)
- **Ignored-but-accepted**: `#include`/preprocessor directives, `/* */` and
  `//` comments

### Not supported

- Pointers and arrays. `int*`, `&x`, `*p` and `a[i]` all fail to compile (only
  the token-level grammar accepts `*` in a declaration); there is no pointer
  arithmetic and `&` is only the bitwise-AND operator. This was a "bonus" item
  in the brief.
- Strings, `struct`/`union`, `float`/`double`, global variables
- `for`, `do…while`, `switch…case`, the ternary `?:` operator
- More than 6 function arguments (no stack-passed parameters)
- Escape sequences in character literals (`'\n'`); use the numeric code
  (`putchar(10)`) instead
- True 8-bit `char` semantics (values are not truncated to a byte)
- Any optimisation pass (constant folding/propagation)

Some of these were optional or "not a priority" in the course brief; the goal was
depth on the core language rather than breadth.

### Known limitations / current bugs

These are honest notes for anyone reading the code, not hidden failures:

- `&&` and `||` return wrong results, e.g. `1 && 1` yields `0` and `1 || 0`
  yields `0`. The lowering tests each operand against `0` using the `cmp_const`
  IR instruction, and `cmp_const` writes only a single byte to its destination
  (`sete dest(%rbp)`) without clearing the other three bytes of the 4-byte slot,
  so the following test reads an ill-defined value. Comparisons such as `a < b`
  are fine (they use `movzbl`/`mov` and write the whole word). This is the cause
  of the 4 failing tests.
- `break` compiles but jumps to the wrong block, so it does not exit its loop.
  (`continue` works.)
- `char` values are allocated 4 bytes and never truncated to a byte, so
  `char c = 300;` does not behave as in C.
- The compiler rejects a program if a declared variable is never used (a strict
  interpretation of one of the brief's static checks), so unused variables and
  unused function parameters are compile errors rather than warnings.
- Function definitions must appear after `main`; a function called from `main`
  needs a forward declaration first.

---

## Repository layout

```
pld-comp/
├── compiler/            # the compiler itself (the interesting part)
│   ├── ifcc.g4          # ANTLR 4 grammar: lexer + parser rules
│   ├── src/             # hand-written C++ sources
│   ├── include/         # headers: IR, visitors, types, errors
│   ├── config/          # per-OS Makefile fragments (ANTLR paths)
│   ├── doc/             # Doxygen configuration
│   └── Makefile         # build, test, gui, doc targets
├── tests/               # functional test harness + test cases
│   ├── ifcc-test.py     # runs gcc and ifcc, compares their behaviour
│   ├── ifcc-wrapper.sh  # how the harness invokes our compiler
│   └── testfiles/       # 163 C programs, grouped by feature
├── multi-target/        # standalone retargeting demo: compiles example.c to x86, ARM, MSP430, JVM
├── docs/                # developer documentation (architecture guide)
├── install-antlr.sh     # downloads and builds ANTLR 4 + C++ runtime locally
├── doc_x86.pdf          # x86 assembly reference manual
├── Sujet.pdf            # original course assignment (French)
└── README.md            # this file
```

See [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) for the developer tour: how a
source file flows through the compiler, what each class does, and how to extend
it.

---

## Building and running

### Prerequisites

- `g++` with C++17, `make`, `gcc`
- **ANTLR 4** plus its **C++ runtime** (`antlr4` + `libantlr4-runtime-dev` on
  Debian/Ubuntu, `antlr4` + `antlr4-cpp-runtime-devel` on Fedora)
- `python3` (test harness)
- `default-jdk` (only needed for the `make gui` parse-tree viewer)

The paths to ANTLR are read from a Makefile fragment in `compiler/config/`
(`ubuntu.mk` by default, `DI.mk` on macOS). Edit the appropriate `.mk` file, or
add your own, if ANTLR lives somewhere else on your machine. If your
distribution has no ANTLR package, run `./install-antlr.sh` from the repository
root - it fetches ANTLR 4.9.2 and builds the C++ runtime under `antlr/`.

### Build

```bash
cd compiler
make            # or: make ifcc
# produces ./build/ifcc
```

### Run

```bash
./build/ifcc path/to/file.c > out.s   # compile C to x86-64 assembly
gcc out.s -o out && ./out             # assemble, link, run
echo $?                               # exit status = value returned by main
```

The generated assembly is printed to **stdout**; this is what you capture into the
`.s` file. Two caveats if you script around the compiler:

- Diagnostics are split. Syntax errors and usage errors go to stderr; the other
  checks (double declaration, reserved keyword, unused variable) go through the
  `error()` macro, which prints to **stdout** (see `Error.h`). An undeclared
  variable, however, exits silently with code 1 and no message.
- `main.cpp` ends with `cerr << EXIT_SUCCESS`, so a successful run also prints a
  stray `0` to stderr. The build defines `TRACE`, so the symbol/code-generation
  debug traces (stderr) are always on as well.

### Testing

```bash
cd compiler
make test     # run the whole suite, show pass/fail
make check    # same, but exit non-zero if any test fails (used by CI)
```

The harness compiles every program in `tests/testfiles/` with both `gcc` and
`ifcc`, runs both binaries, and compares their stdout and exit code. A test also
passes when `ifcc` rejects a program that `gcc` also rejects. Current status:

```
146 tests run - 142 pass, 4 fail (all four are the && / || truthiness bug)
```

Of the 163 programs under `tests/testfiles/`, 146 are run. The 17 skipped ones
live in a dot-prefixed file or directory (e.g. `.46_many_bitwise.c`,
`.15_other_control_structures/280_switch.c`) and are **disabled** tests kept on
purpose: they document features that were planned but not finished (`for`,
`do…while`, `switch`). See `tests/ifcc-test.py --help` for options such as `-v`
(verbose) and `-w` (use a different compiler wrapper).

### Other useful targets

```bash
make gui            # prompts for a test number, then shows that test's parse tree
make verify         # prompts for a test number, then prints ifcc's assembly for it
make doc            # generate Doxygen documentation and open it in a browser
make clean          # remove build/, generated/ and test output
```

### Continuous integration

`.github/workflows/c-cpp.yml` builds the compiler and runs `make check` on every
push and pull request to `main`; `formatter.yml` applies `clang-format` to pull
requests. Because `make check` exits non-zero while the 4 `&&`/`||` tests fail,
the CI job is currently red — a fix for the truthiness bug would turn it green.

---

## Team

The project was carried out by a six-student hexanôme at INSA Lyon. Contributions
span the grammar, the symbol table and semantic checks, the IR and code
generation, and the test suite.

- Gaspard Serpinet — [@2pates](https://github.com/2pates)
- Benjamin Maison — [@BenjaminMaison](https://github.com/BenjaminMaison)
- Mathieu Chevalier — [@m-chevalier](https://github.com/m-chevalier)
- Myriam Jamous — [@MyriamJ17](https://github.com/MyriamJ17)
- Bachir Gueddouda — [@bachirGDD](https://github.com/bachirGDD)
- James Sudlow — [@JamesSudlow](https://github.com/JamesSudlow)

---

## Resources

- [ANTLR 4 C grammar reference](https://github.com/antlr/grammars-v4/blob/master/c/C.g4)
- [`doc_x86.pdf`](doc_x86.pdf) - x86 assembly reference manual
- [`Sujet.pdf`](Sujet.pdf) - original course assignment (in French)
- [Operator precedence chart](operator_priority.png)

## License

MIT - see [LICENSE](LICENSE).
