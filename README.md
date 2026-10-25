# C-Like Compiler Front End

A compiler front end for a C-like language (C with classes, references and `new`/`delete`), implemented with **Flex**, **GNU Bison** and C++17.

The pipeline has four phases:

1. **Lexical analysis** (Flex): recognizes tokens; the standalone `lexer` prints a token / token-type table.
2. **Syntax analysis** (Bison): validates the grammar and reports errors with line numbers.
3. **Semantic analysis**: declaration, type, control-flow, function and access-control checks, reported as errors or warnings.
4. **Intermediate code generation**: three-address code (TAC) for every program that passes the earlier phases.

Machine-code generation is out of scope; the compiler stops at TAC.

## Project Structure

```text
C_Compiler/

├── src/
│   ├── lexer/
│   │   ├── Lexer.l
│   │   ├── Token.cpp
│   │   ├── Token.hpp
│   │   ├── TokenType.hpp
│   │   └── main.cpp            (standalone lexer driver)
│   ├── parser/
│   │   └── parser.y            (grammar + semantic checks + IR generation)
│   ├── semantic/
│   │   └── Types.hpp           (type model used by the semantic checks)
│   └── ir/
│       ├── TAC.hpp
│       └── TAC.cpp             (three-address-code generator)

├── tests/
│   ├── lexer/                  (inputs for make test-lexer)
│   ├── parser/                 (syntax tests, make test-parser)
│   ├── semantic/               (feature programs, make test-semantic / make ir)
│   └── 25_OCT/                 (feature tests, semantic error/warning/valid tests)
│       ├── expected/           (reviewed expected IR, one .tac per feature test)
│       ├── semantic_errors/
│       └── semantic_valid/

├── test.c                      (sample input for make test)

├── docs/
│   └── LANGUAGE_GUIDE.md       (rules, limitations, enums, symbol table, commands, troubleshooting)

├── Makefile
├── run.sh
└── README.md
```

## Requirements

The following tools are required:

* GNU C++ compiler
* Flex
* GNU Bison
* Make

`run.sh` needs a POSIX shell (bash). On Windows, use Git Bash or WSL; running `./run.sh` from PowerShell only opens the file, so use `bash ./run.sh ...` there.

On Fedora, the tools can be installed with:

```bash
sudo dnf install gcc-c++ flex bison make
```

On Debian/Ubuntu (including WSL): `sudo apt install g++ flex bison make`.

## Building

```bash
make            # build lexer and parser (lexer.exe / parser.exe on Windows)
make parser     # build only the compiler
make lexer      # build only the standalone lexer
make clean      # remove binaries, generated sources, ./*.TAC and tests/ir_output/
```

On Windows the executables are linked statically. Application-control or antivirus software may block the first launch of a freshly built `.exe` (symptoms: `Bad file number`, exit status 126, or `UNKNOWN phase` in a `.TAC` file). The Makefile retries once after linking; if a command still fails that way, run it again.

## Running the Compiler

The main submission interface is:

```bash
./parser <source_file>
```

For example:

```bash
./parser tests/parser/test1_types_and_vars.c
```

For a valid program, the parser reports success, prints the three-address-code IR, and writes it to `<source_file>.tac` (or to the path given as a second argument):

```text
=> PARSE SUCCESSFUL: No syntax errors found.
=> SEMANTIC CHECK PASSED: 0 warning(s).
...IR...
=> IR saved to: tests/parser/test1_types_and_vars.tac
```

The token / token-type table is printed by the standalone lexer (`./lexer`, see below).

For invalid input, lexical, syntax or semantic errors are reported instead and no IR is written.

`run.sh` is a convenient wrapper that prints the IR of one file, or of every `.c` file in a folder, and saves each as `./<name>.TAC`:

```bash
./run.sh tests/25_OCT/test14_recursion.c
./run.sh tests/semantic/
```

Example:

```text
SYNTAX ERROR: syntax error at line 3 (near ';')
```

The parser returns a non-zero exit status when an error is detected.

## Running the Standalone Lexer

The lexer can also be executed independently:

```bash
./lexer <source_file>
```

For example:

```bash
./lexer tests/parser/test1_types_and_vars.c
```

The standalone lexer prints each recognized token and its corresponding token type.

Lexical errors are reported to `stderr`, and the lexer returns a non-zero exit status if a lexical error is encountered.

## Test Suite

All test cases live under `tests/` and are run by one script, `run.sh`, which runs the whole suite with `--test <executable>`:

```bash
make                # builds the lexer and parser
./run.sh --test ./parser   # (./parser.exe on Windows) runs every test case; exits 0 only if all of them pass
```

`make check` builds and runs the whole suite. The generated IR is written to `tests/ir_output/` (ignored by git).

### Make targets

| Command              | What it does                                                                                       |
| -------------------- | -------------------------------------------------------------------------------------------------- |
| `make`               | Build the lexer and the parser.                                                                    |
| `make test`          | Generate the IR for `test.c` only, into `./test.TAC`.                                              |
| `make test-lexer`    | Run the lexer over `tests/lexer/*.c`, printing lexical errors (a file with errors is reported, not a make failure). |
| `make test-parser`   | Syntax-check `tests/parser/*.c` (`_invalid` files must be rejected), printing errors.              |
| `make test-semantic` | Semantic-check `tests/semantic/*.c`, printing errors.                                              |
| `make ir`            | IR for every `tests/semantic/*.c`, written to `./<name>.TAC`. A failing file's `.TAC` holds the diagnostics, headed by the failing phase. |
| `make check`         | Full regression suite (`run.sh`).                                                                  |
| `make clean`         | Remove binaries, generated sources, `./*.TAC` and `tests/ir_output/`.                              |

| Folder                              | Tests | What `run.sh` checks                                                                                  |
| ----------------------------------- | ----- | ----------------------------------------------------------------------------------------------------- |
| `tests/parser/`                     | 5     | Syntax. Files whose name contains `_invalid` must be rejected, all others accepted.                   |
| `tests/25_OCT/*.c`                  | 32    | One program per language feature. The generated IR must equal `tests/25_OCT/expected/<name>.tac`.     |
| `tests/25_OCT/semantic_errors/`     | 71    | `err_*.c` must be rejected, `warn_*.c` accepted; stderr must contain the `// EXPECT:` text (line 1).  |
| `tests/25_OCT/semantic_valid/`      | 22    | Tricky and realistic valid programs: accepted, with no diagnostics at all.                            |
| `tests/semantic/`                   | 14    | Must compile (also used by `make test-semantic` and `make ir`).                                       |

`tests/lexer/` holds inputs for the standalone lexer (`make test-lexer`).

If the IR changes on purpose, review it and re-bless the expected files with `./run.sh --update ./parser`.

If the executable cannot be launched at all (for example because Windows is blocking a freshly built `.exe`), `run.sh` stops with a clear message and exit status 2 instead of failing every test.

A successful run (140 tests at the time of writing) ends with:

```text
==============================
Test Summary
==============================
Total:  144
Passed: 144
Failed: 0

All tests passed!
```

### Features covered

| Feature                                  | Test program (`tests/25_OCT/`)                                          |
| ---------------------------------------- | ----------------------------------------------------------------------- |
| All arithmetic and logical operators     | `test01_arith_logical_ops.c`                                            |
| if-else                                  | `test02_if_else.c`                                                      |
| for / while / do-while loop              | `test03_for_loop.c`, `test04_while_loop.c`, `test05_do_while.c`         |
| switch cases                             | `test06_switch_case.c`, `test21_switch_nested_no_default.c`             |
| arrays (integer and char)                | `test07_arrays_int_char.c`, `test24_array_init_forms.c`                 |
| pointers                                 | `test08_pointers.c`, `test22_pointer_compound_ops.c`                    |
| structure                                | `test09_structures.c`, `test23_struct_array_members.c`                  |
| printf and scanf                         | `test10_printf_scanf.c`                                                 |
| function call with arguments             | `test11_function_calls_args.c`                                          |
| goto, break and continue                 | `test12_goto_break_continue.c`                                          |
| static keyword                           | `test13_static_keyword.c`, `test26_static_two_functions.c`              |
| recursive function call                  | `test14_recursion.c`                                                    |
| class and object                         | `test15_class_object.c`, `test25_two_classes_same_method.c`             |
| dynamic memory allocation                | `test16_dynamic_memory.c`                                               |
| public, private and protected            | `test17_access_modifiers.c`, `semantic_errors/err_private_*.c`          |
| enum (with numeric values), union          | `test18_enum_union.c`, `test32_enum_values.c`                           |
| multi-dimensional arrays                 | `test19_multidim_arrays.c`                                              |
| short-circuit `&&` `\|\|`, `?:`            | `test27_short_circuit_ternary.c`                                        |
| references                               | `test28_reference_variables.c` ... `test31_reference_return_sizeof.c`   |
| everything together                      | `test20_integration_bubble_sort.c`                                      |

## Language Features

The parser implements a **C-like language subset** supporting:

### Data Types

```text
int  short  long  signed  unsigned
float  double
char  bool  void
```

plus pointers, references (`int &r = x;`), arrays (including multi-dimensional), `struct`, `union`, `enum` and `class` types, and the `const`, `static`, `extern` and `typedef` specifiers.

### Declarations

```c
int x;

int x = 10;

float value = 3.14;

char c = 'a';

bool flag = true;
```

### Functions

```c
int add(int a, int b) {
    return a + b;
}
```

### Control Flow

* `if` / `else`
* `while`, `do-while`, `for`
* `switch` / `case` / `default`
* `break`, `continue`, `return`
* `goto` and labels

### Expressions

The grammar supports:

* Arithmetic operators
* Assignment and compound assignment
* Relational operators
* Equality operators
* Logical operators
* Bitwise operators
* Shift operators
* Increment/decrement
* Unary operators
* Conditional (`?:`) expressions
* Parenthesized expressions
* Function calls (including recursion)
* Array indexing
* `.` and `->` member access, `this->member`
* Pointer dereference and address-of
* `sizeof`
* `new` / `delete`

Operator precedence and associativity are explicitly defined in the Bison grammar.

### Arrays

```c
int values[10];

values[0] = 5;

values[1]++;
```

### Enums

Each enumerator gets its numeric value at compile time (`NAME` = previous value + 1, starting at 0, or `NAME = constant`), and the IR uses the number: `enum Status { IDLE, RUNNING = 5, STOPPED };` makes `s = RUNNING` become `s = 5` and `STOPPED` become `6`. Decimal, hex, negative and character-literal initialisers are understood. An initialiser that is not a plain literal (for example `1 << 3`) gives a warning, and that enumerator and the ones after it keep their names in the IR. Enumerators cannot be assigned to, and two `case` labels with the same value are rejected.

### Structures and Classes

```c
struct Point {
    int x;
    int y;
};
```

```c
class MyClass {

private:
    int value;

public:
    void setValue(int v) {
        this->value = v;
    }

};
```

Member functions are supported, and all members of a `class` default to `private`. Supported access specifiers:

```text
public
private
protected
```

### Lexical Features

The lexer recognizes:

* Identifiers
* Decimal integer literals
* Hexadecimal integer literals
* Floating-point literals
* Character literals
* String literals
* C-like comments
* C and C++-style operators and punctuators
* C/C++-style keywords supported by the lexer

Lexical errors include:

* Unrecognized characters
* Invalid character literals
* Unterminated character literals
* Unterminated string literals
* Unterminated block comments

## Writing Programs for This Compiler

The compiler accepts a C-like subset. For example, a class or struct name must be written with its keyword (`class Box b1;`, not `Box b1;`), and `typedef` names, anonymous enums, casts and inheritance are not supported. The full list of rules and limitations, how enums and the symbol table work, all commands, and a troubleshooting table are in [docs/LANGUAGE_GUIDE.md](docs/LANGUAGE_GUIDE.md).

## Error Handling

The analyzer reports errors without silently accepting invalid input.

### Syntax Error

```text
SYNTAX ERROR: syntax error at line 3 (near ';')
```

### Lexical Error

```text
LEXICAL ERROR: invalid character literal 'ab' at line 5
```

### Semantic Error

Programs that are syntactically valid but wrong in meaning are rejected, and no IR is written:

```text
SEMANTIC ERROR: 'x' was not declared in this scope at line 2
SEMANTIC ERROR: too few arguments to function 'add' (expected 2, got 1) at line 3
SEMANTIC ERROR: 'secret' is private within this context at line 9

=> SEMANTIC ANALYSIS FAILED: 3 error(s), 0 warning(s). No IR generated.
```

Checks performed:

* **Declarations:** undeclared identifiers, redeclaration, redefinition and conflicting declaration of functions/classes, duplicate members, `void` variables, incomplete struct types, negative array sizes, excess initializers
* **Control flow:** `break`/`continue` outside a loop or switch, undefined or duplicate `goto` labels, `case`/`default` outside a switch, duplicate `case` values, non-integer switch
* **Functions:** wrong argument count, argument/return type mismatch, `return` with a value in a `void` function, calling a non-function, binding a non-const reference to an rvalue
* **Types:** incompatible assignment/initialization (struct vs scalar, pointer vs integer, incompatible pointers), invalid operands (`pointer + pointer`, `float % int`), invalid `*`/`&`/`[]`/`->`/`.` use, assignment to a non-lvalue, a `const` object or an array
* **Classes:** access control (`private`/`protected` members used outside the class), unknown members, `this` outside a member function

Debatable cases are reported as warnings instead and do not stop compilation: implicit declaration of a function, incompatible pointer types, discarded `const`, division by a literal zero, a `return;` in a non-void function.

The checker is conservative: library calls (`printf`, `malloc`, ...), function pointers and anything else whose type it cannot determine are never reported, so valid programs are not rejected.

Lexical, syntax and semantic errors all cause the parser to return a non-zero exit status.

This allows the automated test script to distinguish successfully parsed programs from programs containing errors.

To run all tests (syntax, IR, semantic): `make check`, or `./run.sh --test ./parser`

## Scope

This project implements a **C-like language subset**, rather than the complete ISO C language.

The following are outside the current scope:

* Preprocessor directives and macros (`#include` lines are skipped; common library functions and constants such as `printf`, `malloc` and `NULL` are known to the semantic checker)
* Casts, templates, inheritance and operator overloading
* Machine-code generation and optimization (the compiler stops at three-address-code IR)
* Unicode identifiers
* Wide character/string literals
* Complete ISO C syntax

The lexer may recognize additional C/C++ keywords that are not necessarily used by the current parser grammar.

## Implementation

The project uses:

* **Flex** for lexical analysis
* **GNU Bison/YACC** for syntax analysis
* **C++17** for compilation and supporting code

Bison generates the parser source and header files, while Flex generates the lexer source.

The parser and standalone lexer share the generated lexer implementation. The standalone lexer uses `src/lexer/main.cpp`, while the parser links the generated Bison parser (which holds the semantic checks and drives the TAC generator in `src/ir/`) with the same lexer.

Generated files and compiled binaries are excluded from version control through `.gitignore`.
