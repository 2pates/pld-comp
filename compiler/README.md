# `ifcc` — the compiler

This directory contains the compiler itself. It compiles a subset of C to x86-64
assembly. For the project overview and language coverage, see the
[top-level README](../README.md); for a guided tour of the code, see
[`../docs/ARCHITECTURE.md`](../docs/ARCHITECTURE.md).

## Layout

```
compiler/
├── ifcc.g4              # ANTLR 4 grammar (lexer + parser, one visitor method per operator)
├── src/
│   ├── main.cpp                 # entry point: parse, run both passes, print assembly
│   ├── SymbolGenVisitor.cpp     # pass 1: symbol table, scopes, stack addresses, static checks
│   ├── CodeGenVisitor.cpp       # pass 2: statements, functions, control flow -> IR
│   ├── CodeGenVisitorExpr.cpp   # pass 2: expressions -> IR
│   └── IR.cpp                   # IR classes + x86-64 assembly emission
├── include/             # matching headers (IR.h, SymbolGenVisitor.h, CodeGenVisitor.h, Type.h, Error.h)
├── config/              # ANTLR path fragments per OS: ubuntu.mk, fedora.mk, DI.mk (macOS)
├── doc/                 # Doxygen configuration
└── Makefile
```

`build/` and `generated/` are produced by the build and are git-ignored.
`generated/` holds the C++ files ANTLR creates from `ifcc.g4` (lexer, parser,
base visitor).

## Prerequisites

- `g++` (C++17), `make`, `gcc`
- ANTLR 4 and its C++ runtime. The Makefile reads the paths from
  `config/ubuntu.mk` by default (and `config/DI.mk` on macOS). Edit the relevant
  `.mk`, or add your own and include it, if ANTLR is installed elsewhere.
  On Debian/Ubuntu: `apt install antlr4 libantlr4-runtime-dev`.
  On Fedora: `antlr4` and `antlr4-cpp-runtime-devel`.
  Otherwise, run `../install-antlr.sh` from the repository root to build ANTLR
  4.9.2 locally under `../antlr/`.
- `python3` for the test harness; `default-jdk` for the `gui` target.

## Make targets

| Target | What it does |
|---|---|
| `make` / `make ifcc` | Build the `build/ifcc` executable (runs ANTLR, then GCC) |
| `make test` | Run the full functional test suite (delegates to `../tests`) |
| `make check` | Same as `test`, but exits non-zero if any test fails (used by CI) |
| `make gui` | Prompt for a test number and show that test's ANTLR parse tree in a window (needs `javac`) |
| `make verify` | Prompt for a test number and run `ifcc` on it, printing its assembly |
| `make doc` | Generate Doxygen documentation into `doc/html` and open it in Firefox |
| `make clean` | Remove `build/`, `generated/` and the test output |

## Using the compiler

```bash
./build/ifcc program.c > program.s   # C -> x86-64 assembly (assembly on stdout)
gcc program.s -o program             # assemble + link with libc
./program; echo $?                   # run; exit status is the value main returned
```

The assembly goes to stdout. Syntax errors are reported on stderr; other
diagnostics (double declaration, reserved keyword, unused variable) go through
the `error()` macro, which currently prints to stdout. The compiler exits
non-zero when it rejects a program. The error codes are listed in
`include/Error.h`.

`CCFLAGS` in the Makefile defines `TRACE`, so the compiler prints symbol-table
and code-generation traces to stderr on every run. Remove `-DTRACE` from
`CCFLAGS` (and rebuild) for a quiet compiler; the traces come from the `debug()`
macro in `include/Error.h`.
