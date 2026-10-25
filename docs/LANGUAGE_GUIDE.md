# Language Guide and Compiler Notes

How to write programs for this compiler, what it does with them, and how to run and troubleshoot it.
The overview and the test suite are described in the [README](../README.md).

## 1. Rules and limitations

The compiler accepts a C-like subset. A few things that are legal in C or C++ are not supported; each row shows what to write instead.

| Not supported                                   | Example that fails                          | Write this instead                                  |
| ----------------------------------------------- | ------------------------------------------- | --------------------------------------------------- |
| A bare struct/class/union/enum name as a type   | `Box b1;`, `P p;`                           | `class Box b1;`, `struct P p;`, `enum Color c;`     |
| `typedef` names as types                        | `typedef int myint; myint a;`               | use the underlying type directly                    |
| Anonymous enums                                 | `enum { A, B };`                            | give the enum a name: `enum E { A, B };`            |
| Casts                                           | `(int)3.5`                                  | assign to a variable of the target type             |
| Inheritance, templates, operator overloading    | `class B : public A { };`                   | write the classes independently                     |
| Enumerator values that are not plain literals   | `enum E { A = 1 << 3 };`                    | `A = 8` (otherwise a warning, names stay in the IR) |

A bare type name fails with `SYNTAX ERROR` at the variable name, and every later use of the variable then also reports
`'x' was not declared in this scope`. Fix the first error and the rest disappear.

Other rules:

* **Declare before use.** A variable or function must be declared above its first use. Calling an undeclared function is only a warning (`implicit declaration`); an undeclared variable is an error. Methods of a class may call each other regardless of order.
* **Access control is checked.** Members of a `class` are private by default, those of a `struct` are public. Using a `private`/`protected` member from outside the class is a semantic error.
* **A `switch` needs integer cases.** Duplicate `case` values (including two enumerators with the same value) and a second `default` are errors.
* **Library functions need no declaration.** `printf`, `scanf`, `malloc`, `free`, `NULL` and similar are known to the checker, and `#include` lines are skipped.
* **Errors stop IR generation.** Any lexical, syntax or semantic error means no IR is written. Warnings do not stop it.
* **Calls.** Arguments are pushed right to left, and a method call passes `this` last.

## 2. Enums

Each enumerator gets its number at compile time: `NAME` is the previous value plus one (the first is 0), and `NAME = constant` takes that constant.

```c
enum Status { IDLE, RUNNING = 5, STOPPED };   // IDLE = 0, RUNNING = 5, STOPPED = 6
```

The IR uses the numbers, so `s = RUNNING` becomes `s = 5`, `IDLE + 10` becomes `0 + 10`, and `case STOPPED:` becomes `case 6`.

* Decimal, hex (`0x10`), negative (`-3`) and character (`'A'`) initialisers are understood.
* An initialiser that is not a plain literal (for example `1 << 3`) gives a warning; that enumerator and the ones after it keep their names in the IR.
* Enumerators cannot be assigned to, incremented, or have their address taken (`lvalue required ...`).
* A local variable, parameter or class member with the same name as an enumerator takes priority inside its scope.
* An `enum Tag` variable is treated as an `int`; the compiler does not check that only values of that enum are assigned to it.

## 3. How the symbol table works

The symbol table is built while the parser reads the program (in `src/parser/parser.y`); there is no separate pass over a syntax tree.

* **Scopes.** A stack of maps, one per scope, with the globals at the bottom. A scope is opened for each function body, each `{ }` block and each `for` header. A name is looked up from the innermost scope outwards, which gives shadowing and the `was not declared` error. Declaring a name twice in the same scope gives `redeclaration`.
* **Each entry (`Symbol`)** stores the type (base, pointer levels, array dimensions, struct/class flag, `const`), the array dimensions, the name used in the IR (static locals become `func.var`), and flags for reference, function pointer, `extern`, and enum constant (plus its number).
* **Other tables.** Functions (parameters, return type, defined or not), class/struct/union members with their types and access level, class methods, enum tags, and per-function goto labels and switch case values.
* **Consequence.** Because it is built during parsing, a name must be declared before it is used, as in C. Class methods are the exception: names used inside a method are resolved when the class ends.
* **Not stored.** Scope depth and variable addresses or offsets; the IR refers to variables by name.

## 4. Commands

| Command                | What it does                                                                                              |
| ---------------------- | --------------------------------------------------------------------------------------------------------- |
| `make`                 | Build the lexer and the parser (`lexer.exe` / `parser.exe` on Windows).                                   |
| `make test`            | IR for `test.c` only, written to `./test.TAC`.                                                            |
| `make test-lexer`      | Run the lexer over `tests/lexer/*.c` and print the lexical errors.                                        |
| `make test-parser`     | Syntax-check `tests/parser/*.c` (files with `_invalid` in the name must be rejected).                     |
| `make test-semantic`   | Semantic-check `tests/semantic/*.c` and print the errors.                                                 |
| `make ir`              | IR for every `tests/semantic/*.c`, written to `./<name>.TAC`. A failing file's `.TAC` holds the diagnostics, headed by the failing phase. |
| `make check`           | The full regression suite (`./run.sh --test`).                                                            |
| `make clean`           | Remove binaries, generated sources, `./*.TAC` and `tests/ir_output/`.                                     |
| `./parser file.c [out]`| Compile one file; the IR is printed and saved to `file.tac`, or to `out`.                                 |
| `./lexer file.c`       | Print the token table of one file.                                                                        |
| `./run.sh file.c`      | Print the IR of one file (or every `.c` in a folder) and save it as `./<name>.TAC`.                       |
| `./run.sh --test ./parser`   | Run the whole test suite; exits 0 only if every test passes.                                        |
| `./run.sh --update ./parser` | Re-save the expected IR in `tests/25_OCT/expected/` (only after reviewing the new IR).              |

## 5. Troubleshooting

| Symptom                                                              | Cause and fix                                                                                          |
| -------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ |
| `SYNTAX ERROR ... near 'b1'` followed by `'b1' was not declared`     | A bare class/struct name was used as a type. Write `class Box b1;` (see section 1).                    |
| `Bad file number`, `Permission denied` or exit status 126 when running `parser.exe` / `lexer.exe` | Windows security (Smart App Control / application control) blocks unsigned, freshly built executables. Run the command again; if it keeps happening, turn off Smart App Control or use WSL. |
| `UNKNOWN phase` inside a `.TAC` file                                 | The same blocking: the compiler never ran, so no phase could be named.                                 |
| `run.sh` prints "cannot be launched" and exits with status 2         | The executable is blocked (see above) or was not built; run `make` first.                              |
| Running `./run.sh` from PowerShell only opens the file               | Use Git Bash or WSL, or `bash ./run.sh ...`.                                                           |
| `make: Circular parser <- parser dependency dropped`                 | Fixed in the current Makefile. If you still see it, you have an old copy of the Makefile.              |
| `IR differs from expected` in the test run                           | The IR changed. If that was intended, review the diff and re-save with `./run.sh --update ./parser`.   |
