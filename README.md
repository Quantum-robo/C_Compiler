# C-Like Syntax Analyzer

A syntax analyzer for a C-like programming language implemented using **Flex** and **GNU Bison**.

The project contains a lexical analyzer that recognizes tokens and a Bison-based parser that validates the syntax of source programs. The parser reports syntax and lexical errors with line information and prints a token/token-type table for syntactically valid programs.

## Project Structure

```text
C_Compiler/
├── src/
│   ├── lexer/
│   │   ├── Lexer.l
│   │   ├── Token.cpp
│   │   ├── Token.hpp
│   │   └── main.cpp
│   └── parser/
│       └── parser.y
├── tests/
│   └── parser/
│       ├── test1_types_and_vars.c
│       ├── test2_control_flow.c
│       ├── test3_functions.c
│       ├── test4_oop_and_structs.c
│       ├── test5_invalid_syntax.c
│       ├── test6_expressions_and_arrays.c
│       └── test7_invalid_lexical.c
├── scripts/
│   ├── run_lexer.sh
│   └── run_parser.sh
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

On Fedora, they can be installed with:

```bash
sudo dnf install gcc-c++ flex bison make
```

## Building

Build both the lexer and syntax analyzer:

```bash
make
```

Build only the parser:

```bash
make parser
```

Build only the standalone lexer:

```bash
make lexer
```

Remove generated files and executables:

```bash
make clean
```

## Running the Syntax Analyzer

The main submission interface is:

```bash
./parser <source_file>
```

For example:

```bash
./parser tests/parser/test1_types_and_vars.c
```

For a syntactically valid program, the parser prints a table containing:

```text
Token                | Token_Type
---------------------|----------------------
int                  | TOKEN_KW_INT
main                 | TOKEN_IDENTIFIER
(                    | TOKEN_LEFT_PAREN
...
```

and finishes with:

```text
=> PARSE SUCCESSFUL: No syntax errors found.
```

For invalid input, syntax or lexical errors are reported instead.

Example:

```text
SYNTAX ERROR: syntax error at line 3 (near ';')
```

The parser returns a non-zero exit status when an error is detected.

## Test Suite

Seven parser test cases are included.

| Test                             | Purpose                                                                   |
| -------------------------------- | ------------------------------------------------------------------------- |
| `test1_types_and_vars.c`         | Data types, declarations, literals, initialization                        |
| `test2_control_flow.c`           | `if`, `else`, `while`, `for`, `break`, increment/decrement                |
| `test3_functions.c`              | Function definitions, parameters, return statements                       |
| `test4_oop_and_structs.c`        | Classes, structs, access specifiers, `this->`                             |
| `test5_invalid_syntax.c`         | Missing expressions, parentheses, and semicolons                          |
| `test6_expressions_and_arrays.c` | Expressions, operators, arrays, function calls, conditional operator      |
| `test7_invalid_lexical.c`        | Invalid characters, character literals, and unterminated strings/comments |

Run the complete parser test suite using:

```bash
./run.sh ./parser
```

The script automatically runs every `.c` file in `tests/parser/` and distinguishes valid tests from tests whose filenames contain `_invalid_`.

A successful run reports:

```text
Total:  7
Passed: 7
Failed: 0

All parser tests passed!
```

There are also development-specific test commands:

```bash
make test-parser
make test-lexer
```

## Language Features

The parser implements a C-like language subset supporting:

### Data Types

```text
int
float
double
char
bool
void
```

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
* `while`
* `for`
* `break`
* `continue`
* `return`

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
* Function calls

Operator precedence is explicitly defined in the Bison grammar.

### Arrays

```c
int values[10];

values[0] = 5;
values[1]++;
```

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

Supported access specifiers include:

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

## Error Handling

The analyzer reports errors without silently accepting invalid input.

### Syntax error

```text
SYNTAX ERROR: syntax error at line 3 (near ';')
```

### Lexical error

```text
LEXICAL ERROR: invalid character literal 'ab' at line 2
```

The parser sets an error status when either lexical or syntax errors are detected.

## Scope

This project implements a **C-like language subset**, rather than the complete ISO C language.

The following are outside the current scope:

* Preprocessor directives and macros
* Symbol-table construction
* Semantic/type checking
* Code generation
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

Generated files and compiled binaries are excluded from version control through `.gitignore`.
