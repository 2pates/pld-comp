# Architecture

This document is the developer tour of `ifcc`. It explains how a C source file
becomes x86-64 assembly, what each class is responsible for, and where to make
changes. It assumes you have read the top-level [`README.md`](../README.md).

---

## Big picture

```
  file.c
    |
    v
  ANTLR lexer (ifccLexer)  -->  ANTLR parser (ifccParser)  -->  parse tree
                                                                   |
                                              two passes over the same tree
                                        +--------------------------+--------------------------+
                                        |                                                     |
                                        v                                                     v
                        +---------------------------+                      +---------------------------+
                        | SymbolGenVisitor (pass 1) |                      | CodeGenVisitor (pass 2)   |
                        | symbol table, scopes,     |                      | AST -> IR / CFG           |
                        | stack addresses, checks   |                      +---------------------------+
                        +---------------------------+                                    |
                                        |                                              v
                                        | symbol table                 +---------------------------+
                                        +----------------------------> | CFG: BasicBlock + IRInstr |
                                                                       | emits x86-64 assembly     |
                                                                       +---------------------------+
                                                                                   |
                                                                                   v
                                                          x86-64 assembly on stdout --> gcc --> executable
```

The compiler is a **two-pass** design. Pass 1 (`SymbolGenVisitor`) computes
everything that must be known before generating code — in particular the stack
address of every variable and temporary. Pass 2 (`CodeGenVisitor`) walks the same
tree again and emits IR instructions into basic blocks. Finally the CFG prints
itself as assembly.

Two passes are used because x86 stack addresses are assigned downward from
`%rbp` and a variable's address must be known before any instruction that reads
or writes it. Resolving all declarations first keeps code generation simple.

---

## Source map

| File | Role |
|---|---|
| `compiler/ifcc.g4` | ANTLR 4 grammar: lexer rules (tokens) and parser rules, including operator precedence |
| `compiler/src/main.cpp` | Entry point: read file, lex, parse, run the two visitors, emit assembly |
| `compiler/include/SymbolGenVisitor.h` / `src/SymbolGenVisitor.cpp` | Pass 1: symbol table, scopes, addresses, static checks |
| `compiler/include/CodeGenVisitor.h` / `src/CodeGenVisitor.cpp` / `src/CodeGenVisitorExpr.cpp` | Pass 2: AST → IR/CFG |
| `compiler/include/IR.h` / `src/IR.cpp` | IR data structures (`IRInstr`, `BasicBlock`, `CFG`) and x86-64 emission |
| `compiler/include/Type.h` | `Type` enum (`INT32`, `CHAR8`, `POINTER`) |
| `compiler/include/Error.h` | Error codes, `error()` and `debug()` macros |
| `compiler/config/*.mk` | Per-OS ANTLR paths (`ubuntu.mk`, `fedora.mk`, `DI.mk` for macOS) |

The grammar-generated files (`ifccLexer.cpp`, `ifccParser.cpp`,
`ifccBaseVisitor.cpp`, …) live in `compiler/generated/`, which is not committed
and is recreated by `make`.

---

## Stage by stage

### 1. Lexing and parsing (`ifcc.g4`)

The grammar is a single file with both lexer and parser rules.

- Tokens: `VARNAME`, `FUNCNAME`, `CONST_INT`, `CONST_CHAR`, `CONST_VOID`
  (`NULL`), `RETURN`, and rules that are **skipped** (`COMMENT`, `ONE_LINE_COMMENT`,
  `DIRECTIVE`) or hidden (`WS`).
- The parser rule `expr` encodes C precedence and left-associativity by ordering
  the alternatives from **highest to lowest** precedence (ANTLR gives the first
  listed alternative priority). Every alternative is **labelled** (`#expr_add`,
  `#expr_mult`, `#expr_atom`, …), which makes ANTLR generate one visitor method
  per operator instead of one giant `visitExpr`. Note that the unary rule
  `#expr_unaire` covers only `-`, `~` and `!`; there is no unary `*` or `&`, which
  is why pointer expressions do not parse.
- The top-level rule is `axiom: prog EOF`, and `prog` fixes the shape of a
  program: `int main ( ) block function_def*`. This is why function definitions
  must follow `main`.

`main.cpp` runs the lexer, fills a token stream, and calls `parser.axiom()`. If
`getNumberOfSyntaxErrors()` is non-zero the compiler exits with an error.

### 2. Pass 1 — `SymbolGenVisitor`

This pass walks the tree and builds the **symbol table**, a
`std::unordered_map<std::string, VariableInfo>`. `VariableInfo` holds:

- `address`: offset relative to `%rbp` (negative — the stack grows down),
- `size`: size in bytes (4 for `int`/`char`, 8 for pointers),
- `defined`, `used`: flags used by the static checks.

Key mechanics:

- **Scopes and shadowing.** Every variable is stored under a mangled name
  `name#function_block` (see `create_unique_var_name`). A block is numbered by
  `tmp_block_index`; `blocks` maps each block to its parent. Lookups walk that
  chain (`check_exist_in_current_or_parent_block`), which gives lexical scoping
  and lets an inner declaration shadow an outer one.
- **Stack allocation.** `memory_offset` starts at 0 and is decremented by 4
  (or 8 for pointers) for each declaration. The final value becomes
  `CFG::memoryUse`, which sizes the stack frame.
- **Temporaries.** Intermediate results get generated names `#tmp<n>` and their
  own stack slots, so every IR operand has a memory location.
- **Static checks.** Double declaration, use of an undeclared variable, and use
  of a `void` variable are detected here and reported via `error(...)` followed
  by `exit(...)` with a code from `Error.h`.

`main.cpp` then scans the table and errors out on any variable whose `used` flag
is false (unless its name starts with `#`, i.e. it is a compiler temporary).

### 3. Pass 2 — `CodeGenVisitor` → IR

`CodeGenVisitor` walks the tree a second time and appends `IRInstr`s to the
current `BasicBlock`. It uses the same block numbering and mangled names as pass
1, so the two passes agree on addresses.

The IR is **three-address**: every operation reads named operands and writes a
named destination, and each name maps to a stack slot. `IRInstr::Operation`
enumerates the opcodes (`ldconst`, `copy`, `add`, `cmp_lt`, `call`, `jump`,
`lazy_and`, …). `IR.cpp` documents the operand order of each opcode in a comment
block at the top of the file.

Expressions are handled in `CodeGenVisitorExpr.cpp`. Each `visitExpr_*` method
evaluates its sub-expressions, allocates a fresh `#tmp`, emits the matching IR
instruction, and returns the temporary's name. Constants become `ldconst`
instructions; variables are looked up through the scope chain.

Control flow builds the **control-flow graph**:

- `if` creates test/true/false blocks and sets `exit_true` / `exit_false`
and `test_var_name`.
- `while` creates a test block and a body block and links them back.
- `return expr;` evaluates `expr` and emits a single `ret` instruction in the
  current block (there is no shared exit block). Each function also gets an
  implicit `ret` appended after its body, so falling off the end returns 0 for
  non-`void` functions.
- Function calls emit one `copyIn` per argument
  (into the ABI registers, up to 6) followed by a single `call`. `putchar` and
  `getchar` are special-cased in `visitFunction_call`.

`&&` and `||` are lowered to short-circuit basic blocks in
`visitExpr_lazy_and` / `visitExpr_lazy_or`. Note the known truthiness bug
described in the README: the result is computed with `cmp_const`/`l_not` on the
raw integer values rather than a normalisation to `0`/`1`.

### 4. Code generation — `IR.cpp`

`CFG::gen_asm` emits the prologue, then asks every `BasicBlock` to emit itself.
`BasicBlock::gen_asm` calls `IRInstr::gen_asm` for each instruction and then
emits the branch based on `exit_true` / `exit_false` / `test_var_name`.

`IRInstr::gen_asm` is a `switch` over the opcode, one `case` per operation, each
printing a few x86-64 lines. Two things keep it readable:

- `size_to_letter(size)` maps a byte size to the AT&T instruction suffix (`b`/`l`/`q`),
  so the same code works for 8-, 32- and 64-bit operands.
- Operands are named IR values; `CFG::get_var_info(name)` returns the
  `VariableInfo` whose `address` is the `offset(%rbp)` memory operand to print.

The prologue is emitted by `CFG::gen_asm_prologue`: it sets up `%rbp` and reserves
a frame rounded up to 16 bytes (the ABI requires 16-byte stack alignment before a
`call`). There is no separate epilogue method — the `ret` IR instruction emits the
function exit itself (restoring `%rsp`/`%rbp`, then `ret`). The ABI register order
used for arguments is the `repList` array in `IR.h`
(`%edi, %esi, %edx, %ecx, %r8d, %r9d`, with `%eax` for the return value).

`Target` is an enum with a single member, `x86`; every `gen_asm` is written as
`if (target == Target::x86)`, which is the hook where a second back-end would be
added.

---

## Adding a feature

A typical feature (say, a new operator) touches three places:

1. **Grammar** — add or reuse an alternative in `ifcc.g4` with a `#label`.
2. **IR** — if the operation is new, add an `Operation` to `IRInstr` and a
   `case` in `IRInstr::gen_asm`; document the operand order in `IR.cpp`.
3. **Visitor** — implement the matching `visitExpr_<label>` in
   `CodeGenVisitorExpr.cpp`: visit the children, allocate a `#tmp`, emit the IR
   instruction, return the temporary name.

Add a test under `tests/testfiles/` and run `make check`.

---

## Testing model

`tests/ifcc-test.py` is the reference harness. For each `.c` file it:

1. compiles with `gcc -S -Wall -Werror` (the reference compiler),
2. compiles with `ifcc` (through `tests/ifcc-wrapper.sh`, which adapts the CLI
   and forwards the exit status),
3. links and runs both binaries and compares stdout and exit code.

A test passes if both compilers accept the program and the runs agree, **or** if
both compilers reject it. Paths containing a dot-prefixed file or directory (e.g.
`.46_many_bitwise.c` or the `.15_other_control_structures/` directory) are
skipped; they are kept as documentation of unfinished work. `make check` fails
the build if any test fails, which is what CI runs.

---

## Known rough edges

- `char` is allocated 4 bytes and arithmetic on it is 32-bit; the IR has a
  `CHAR8` type but the back-end does not use 8-bit registers yet, so values are
  never truncated to a byte.
- Pointers are only half-wired: the grammar accepts `*` in a declaration
  (`lvalue: pointer_type? VARNAME`) and pass 1 allocates 8 bytes for it, but the
  expression grammar has no unary `&`/`*`, so real pointer programs do not
  compile.
- Only one `Target` (`x86`) exists. The retargeting idea is explored in the
  separate `multi-target/` directory, but that is a standalone demonstration: it
  compiles a fixed `example.c` with each platform's native toolchain (or a manual
  C→Java rewrite) rather than reusing `ifcc`'s IR and back-end. There is no ARM,
  MSP430 or JVM back-end in the compiler itself.
- `&&` / `||` are wrong for operands other than `0`/`1`. The culprit is the
  `cmp_const` opcode: it emits `sete dest(%rbp)`, which stores one byte and
  leaves the rest of the 4-byte slot untouched, so the subsequent truth test
  reads garbage. Fixing `cmp_const` (e.g. `sete %al; movzbl %al, %eax; movl
  %eax, dest(%rbp)`, as the comparison opcodes do) should fix the 4 failing
  tests.
- `break` jumps to the wrong block, so it does not exit its loop.
- The `lazy_and` / `lazy_or` opcodes and their `case`s in `IRInstr::gen_asm` are
  dead code (with unfinished, empty-label branches). `&&` and `||` are actually
  lowered to basic blocks in `visitExpr_lazy_and` / `visitExpr_lazy_or`.
- Some source blocks in `SymbolGenVisitor.cpp` are commented out; they are
  historical and can be ignored.
