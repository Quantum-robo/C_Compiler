# C-Like Compiler: C / C++ subset to MIPS32

A compiler written in **C++** with **Flex** and **GNU Bison**. It translates a C-like language (C with classes,
constructors / destructors, references and `new` / `delete`) into **MIPS32 assembly** that runs on the
**SPIM simulator**. The intermediate representation is **three-address code**.

| Item                     | Choice                                              |
| ------------------------ | --------------------------------------------------- |
| Implementation language  | C++17, Flex (lexer), Bison (parser)                 |
| Source language          | C-like language (C plus classes and references)     |
| Intermediate representation | Three-address code (TAC)                         |
| Target                   | MIPS32 assembly                                     |
| **Simulator required**   | **SPIM** (`spim -file prog.s`); QtSpim and MARS load the same file |

```text
source.c -> Lexer -> Parser + semantic checks -> TAC (typed) -> MIPS code generator -> source.s
                                                                  + runtime library (printf, scanf, malloc, ...)
```

## Requirements

* a C++ compiler (`g++`), `flex`, `bison` and `make` to build the compiler;
* **`spim`** to run the generated programs and the end-to-end tests.

```bash
sudo apt install g++ flex bison make spim        # Debian / Ubuntu (also in WSL)
```

On Windows use WSL, or Git Bash for the shell scripts; a freshly built `.exe` may be blocked for a moment by
application control (the Makefile retries once, see [docs/LANGUAGE_GUIDE.md](docs/LANGUAGE_GUIDE.md) for troubleshooting).

## Build, compile, run

```bash
make                              # builds ./compiler (and the standalone ./lexer)
./compiler prog.c                 # writes prog.s (MIPS assembly)
spim -file prog.s                 # run it on the simulator
```

Options of `./compiler`:

| Option            | Meaning                                                              |
| ----------------- | -------------------------------------------------------------------- |
| `-o <file>`       | write the assembly to `<file>` (default: `<source>.s`)               |
| `--check`         | check only (syntax, types, IR); write no assembly                    |
| `--tac <file>`    | also write the three-address code to `<file>`                        |
| `--print-tac`     | print the three-address code                                         |
| `--tables`        | add the struct layouts and symbol tables to the TAC output           |

The exit status is 0 on success, 1 for a compile error, 2 for a bad command line. `main`'s return value becomes
the exit status of the program under SPIM; `main(int argc, char **argv)` receives SPIM's arguments.

The generated `.s` file is self-contained: it holds the program and the part of the runtime library it uses.

## Tests

```bash
make check                        # same as:  ./run.sh ./compiler
./run.sh ./compiler               # runs every test in tests/ (the end-to-end tests are skipped without spim)
make check-archived               # the older front-end suite in archived_tests/
```

`run.sh` takes the compiler executable as its argument and prints one PASS / FAIL line per test and a summary.

| Folder              | Tests | What is checked                                                                          |
| ------------------- | ----- | ---------------------------------------------------------------------------------------- |
| `tests/codegen/`    | 47    | **End to end**: compile, run on SPIM, compare the output with `<name>.expected` (stdin from `<name>.in`). The expected output was produced by building the same source with g++ |
| `tests/errors/`     | 22    | Programs that must be **rejected** (private / protected access, undeclared names, wrong constructor calls, ...) with the expected message |
| `tests/ir/`         | 5     | The three-address code, struct layouts and symbol tables equal `expected/<name>.tac`      |
| `tests/parser/`     | 5     | Syntax: files with `_invalid` in the name are rejected, the others accepted               |
| `tests/semantic/`   | 14    | Feature programs that must compile                                                       |
| `tests/lexer/`      | 5     | Inputs for the standalone lexer (`make test-lexer`)                                      |
| `archived_tests/`   | 125   | The earlier front-end suite (golden TAC, semantic error and warning programs)           |

`./run.sh --update ./compiler` re-saves the expected TAC of `tests/ir` and the expected output of `tests/codegen` after a
change that was reviewed. `scripts/long_functions.py` lists functions longer than 30 lines that have no comment.

### Features and the tests that cover them

| Feature                                   | Test programs in `tests/codegen/`                                  |
| ----------------------------------------- | ------------------------------------------------------------------ |
| All arithmetic and logical operators      | `01_arith_logical`, `30_operators_edge`                            |
| if / else                                 | `02_if_else`                                                       |
| for, while, do-while                      | `03_for`, `04_while`, `05_do_while`                                |
| switch                                    | `06_switch`, `31_control_edge`                                     |
| Arrays of int and char                    | `07_arrays`, `32_array_edge`                                       |
| Pointers (also multi-level, function pointers) | `08_pointers`, `35_pointer_edge`                              |
| Structures                                | `09_structs`, `33_struct_edge`                                     |
| printf and scanf                          | `10_printf_scanf`, `25_printf_formats`, `28_scanf_formats`, `42_sprintf` |
| Function calls with arguments             | `11_func_args`                                                     |
| Recursion                                 | `12_recursion`, `36_recursion_edge`                                |
| Classes and objects                       | `13_class_object`, `34_class_edge`, `19_access_modifiers`, `22_bank_class` |
| Constructors and destructors              | `43_ctor_dtor`, `44_ctor_members`, `45_raii`                       |
| Dynamic memory (new, delete, malloc, free)| `14_dynamic_memory`, `27_malloc_free`, `37_dynamic_edge`, `21_linked_list` |
| References                                | `15_references`, `38_reference_edge`                               |
| Multi-dimensional arrays                  | `16_multidim_arrays`, `39_multidim_edge`                           |
| goto, break, continue                     | `17_goto_break_continue`                                           |
| static                                    | `18_static`                                                        |
| public / private / protected              | `19_access_modifiers` (legal use), `tests/errors/` (illegal use)   |
| enum, union                               | `20_enum_union`, `40_enum_union_static`                            |
| Strings and the C library                 | `26_strings_ctype`, `27_malloc_free`, `41_macros_casts_names`      |
| float and double, `%f` in printf / scanf, math functions | `46_float`, `47_float_edge`                  |

## The language

C with classes, references and `new` / `delete`: all the usual operators and statements, `int` / `char` / `short` / `long` /
`bool`, `float` / `double`, pointers (including pointers to pointers and to functions), arrays of any dimension, structs, unions, enums,
classes with `public` / `private` / `protected`, methods, constructors (initializer lists, overloading by argument
count) and destructors, references, casts, `static`, `goto`, `#define NAME value` macros, and a runtime library with
`printf` / `scanf` / `sprintf` (including `%f %e %g`), `malloc` / `free`, and the string, ctype and math functions.

**Not supported:** `long double`, inheritance, virtual functions, templates, operator
overloading, `typedef`, function-like macros, unsigned arithmetic, file I/O. The complete list, with the reasons and
the workarounds, is in [docs/LANGUAGE_GUIDE.md](docs/LANGUAGE_GUIDE.md).

## Project structure

```text
C_Compiler/
├── src/
│   ├── lexer/        Lexer.l, Token.*, main.cpp          (tokens; macros; type-name hook; standalone lexer)
│   ├── parser/       parser.y                            (grammar, semantic checks, IR generation, driver)
│   ├── semantic/     Types.hpp, Layout.*                 (type model; sizes and field offsets)
│   ├── ir/           TAC.hpp, TAC.cpp                    (three-address code, symbol tables)
│   └── codegen/      MipsGen.*, Runtime.cpp              (TAC -> MIPS32; runtime library)
├── tests/            codegen/ errors/ ir/ parser/ semantic/ lexer/
├── archived_tests/   25_OCT/                              (earlier front-end suite)
├── docs/             LANGUAGE_GUIDE.md  IR.md  MIPS.md
├── scripts/          long_functions.py
├── Makefile          builds ./compiler and ./lexer; make check
├── run.sh            runs the tests with the executable given as its argument
└── README.md
```

How the compiler works: the program is read twice (the first pass only collects the layout of every class, so a method
can use members declared after it), semantic checks and IR generation happen in the Bison actions, and the typed
three-address code is then translated to MIPS. See [docs/IR.md](docs/IR.md) for the IR and [docs/MIPS.md](docs/MIPS.md) for
the code generator, the calling convention and the runtime library (which is itself written in this language and
compiled with the program).

## Makefile targets

| Command              | What it does                                                                         |
| -------------------- | ------------------------------------------------------------------------------------ |
| `make`               | Build `./compiler` and `./lexer`.                                                    |
| `make check`         | Run the test suite (`./run.sh ./compiler`).                                          |
| `make check-archived`| Run the archived front-end suite.                                                    |
| `make test`          | Compile `test.c` to `test.s` and run it on SPIM.                                     |
| `make test-lexer`    | Run the standalone lexer over `tests/lexer/*.c`.                                     |
| `make test-parser`   | Syntax-check `tests/parser/*.c`.                                                     |
| `make test-semantic` | Check `tests/semantic/*.c`.                                                          |
| `make ir`            | Write `./<name>.TAC` (three-address code) for every `tests/semantic/*.c`.            |
| `make clean`         | Remove the binaries, the generated sources and the generated files.                  |
