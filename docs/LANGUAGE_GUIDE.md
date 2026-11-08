# Language Guide and Compiler Notes

How to write programs for this compiler, what it does with them, and how to run and troubleshoot it.
The overview and the test suite are described in the [README](../README.md).

## 1. Rules and limitations

The compiler accepts a C-like subset (C with classes, references and `new` / `delete`). These things are legal in C or
C++ but are **not supported**; each row shows what to write instead.

| Not supported                                   | Example that fails                          | Write this instead                                  |
| ----------------------------------------------- | ------------------------------------------- | --------------------------------------------------- |
| Temporary objects `Counter(5)`, copy constructors for by-value copies | `f(Counter(5))`  | declare a named object (a by-value copy copies the bytes) |
| Inheritance, virtual functions, templates, operator overloading | `class B : public A { };`   | write the classes independently                     |
| `typedef`                                       | `typedef int myint;`                        | use the underlying type (a `#define` works for a name) |
| Function-like macros                            | `#define MAX(a, b) ...`                     | write a function                                    |
| Floating point (`float`, `double`)              | `float f = 1.5;`                            | not yet supported by the code generator (see MIPS.md) |
| Unsigned arithmetic                             | `unsigned int u;` is accepted but signed    | use `int`; `(unsigned char)` / `(unsigned short)` casts do mask |
| Pointers to arrays                              | `int (*row)[3] = grid;`                     | `int *row = grid[1];` or index the array            |
| `cond ? a : b` as an lvalue                     | `return c ? x : y;` in a function returning `int &` | use `if` / `else`                            |
| Enumerator values that are not plain literals   | `enum E { A = 1 << 3 };`                    | `A = 8` (otherwise a warning, names stay in the IR) |
| `sscanf`, `fprintf`, `fopen`, math functions     | `sqrt(2)`                                   | not provided by the runtime library                 |

Also accepted since they are common: `class B;` forward declarations, anonymous `struct { } name;` / `union { } name;`
members and anonymous `enum { A, B };`, casts `(int*)malloc(n)`, `(char)x`, `(bool)n`, and object-like macros
(`#define SIZE 10`, expanded wherever the name appears; `#include` and conditional-compilation lines are ignored).
A struct, class, union or enum name can be used as a type without the keyword (`Counter c;`, `Node *n;`), and a variable
may even share its name with a type (`struct node *node;`).

Other rules:

* **Declare before use.** A variable or function must be declared above its first use. Calling an undeclared function is only a warning (`implicit declaration`); an undeclared variable is an error. Methods of a class may call each other regardless of order.
* **Access control is checked.** Members of a `class` are private by default, those of a `struct` are public. Using a `private`/`protected` member from outside the class is a semantic error.
* **A `switch` needs integer cases.** Duplicate `case` values (including two enumerators with the same value) and a second `default` are errors.
* **Library functions need no declaration.** `printf`, `scanf`, `malloc`, `free`, `NULL` and similar are known to the checker, and `#include` lines are skipped.
* **Errors stop IR generation.** Any lexical, syntax or semantic error means no IR is written. Warnings do not stop it.
* **Calls.** Arguments are pushed right to left, and a method call passes `this` last. They are evaluated left to right, so do not rely on the order of side effects between arguments.
* **Array sizes are constants.** `int a[10]`, `int a[N]` with an enumerator `N`, and `const int N = 10; int a[N];` are accepted; a size that depends on a run-time value is an error. The first size may be left out when there is an initializer (`int a[] = {1, 2, 3};`).
* **Variables get unique IR names.** A second `i` in a sibling block becomes `i.2`, and a local that shadows a global is renamed (`x.1`); the IR therefore never needs scope information (see [IR.md](IR.md)).
* **Sizes.** `char` is 1 byte, `short` 2, `int`, `long`, `float` and pointers 4, `double` 8. Struct members are aligned to their own size, and `sizeof` is evaluated by the compiler.

## 1b. Constructors and destructors

```c
class Account {
public:
    int balance;
    Account(int start) : balance(start) { }          // constructor with an initializer list
    Account() { balance = 0; }                        // overloaded by the number of parameters
    ~Account() { printf("closing %d\n", balance); }  // destructor
};
Account a(100);        // constructor with arguments        Account b;   // default constructor
Account *p = new Account(5);     delete p;     Account list[3];     // every element gets the default constructor
```

* **When constructors run:** at the declaration of a local, global or static object (a global one from `_global_init`),
  for every element of an array of objects, and for `new T`, `new T(args)` and `new T[n]`. A constructor is chosen by
  its **number of arguments**; two constructors with the same number of parameters are an error.
* **Members:** members that are objects are constructed first, in declaration order, using the arguments of the
  initializer list (`: a(1), b(2)`) or their default constructor, then the body runs. A class whose members need
  construction but which has no constructor gets an implicit default one (likewise for the destructor).
* **When destructors run:** at the end of the scope of a local object, in reverse order of declaration, also when the
  scope is left by `return`, `break` or `continue`; for global objects after `main` returns (last constructed first);
  for `delete` and `delete[]`; and for the members of an object after its own destructor body.
* **Not supported:** `goto` out of a scope does not run destructors; temporary objects (`Counter(5)` as an expression);
  copy constructors are never called implicitly (a by-value copy copies the bytes, and a by-value parameter is
  destroyed when the function returns); a returned local object is moved out like a C++ return-value optimisation;
  static locals are constructed at program start, not at the first call; constructors defined outside the class body.

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

| Command                       | What it does                                                                                     |
| ----------------------------- | ------------------------------------------------------------------------------------------------ |
| `make`                        | Build `./compiler` and `./lexer` (`.exe` on Windows).                                            |
| `make check`                  | The test suite in `tests/` (`./run.sh ./compiler`).                                              |
| `make check-archived`         | The archived front-end suite in `archived_tests/25_OCT/` (`./run.sh --archived ./compiler`).     |
| `make test`                   | Compile `test.c` to `test.s` and run it on SPIM.                                                 |
| `make test-lexer`             | Run the standalone lexer over `tests/lexer/*.c` and print the lexical errors.                    |
| `make test-parser`            | Syntax-check `tests/parser/*.c` (files with `_invalid` in the name must be rejected).            |
| `make test-semantic`          | Semantic-check `tests/semantic/*.c` and print the errors.                                        |
| `make ir`                     | Three-address code for every `tests/semantic/*.c`, written to `./<name>.TAC`.                    |
| `make clean`                  | Remove binaries, generated sources, `./*.TAC` and `tests/ir_output/`.                            |
| `./compiler prog.c`           | Compile to `prog.s`; run it with `spim -file prog.s`. `-o file` names the output.                |
| `./compiler --check prog.c`   | Only check the program (syntax, types, IR). `--tac file`, `--print-tac`, `--tables` show the IR. |
| `./lexer prog.c`              | Print the token table of one file.                                                               |
| `./run.sh ./compiler`         | Run the whole test suite; exits 0 only if every test passes.                                     |
| `./run.sh file.c`             | Print the IR of one file (or every `.c` in a folder) and save it as `./<name>.TAC`.              |
| `./run.sh --update ./compiler`| Re-save the expected IR (`tests/ir`) and expected output (`tests/codegen`) after a reviewed change. |
| `./run.sh --update --archived ./compiler` | Re-save the expected IR in `archived_tests/25_OCT/expected/`.                        |

## 5. Troubleshooting

| Symptom                                                              | Cause and fix                                                                                          |
| -------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ |
| `Bad file number`, `Permission denied` or exit status 126 when running `compiler.exe` / `lexer.exe` | Windows security (Smart App Control / application control) blocks unsigned, freshly built executables. Run the command again; if it keeps happening, turn off Smart App Control or use WSL. |
| `UNKNOWN phase` inside a `.TAC` file                                 | The same blocking: the compiler never ran, so no phase could be named.                                 |
| `run.sh` prints "cannot be launched" and exits with status 2         | The executable is blocked (see above) or was not built; run `make` first.                              |
| Running `./run.sh` from PowerShell only opens the file               | Use Git Bash or WSL, or `bash ./run.sh ...`.                                                           |
| `IR differs from expected` in the test run                           | The IR changed. If that was intended, review the diff and re-save with `./run.sh --update ./compiler`.   |
