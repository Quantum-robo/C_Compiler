# Lexical Analyzer

Flex-based lexer for a C-like language with minimal OOP extensions.

## Build
```
cd C_Compiler
make
```
### Test
To run all test cases :
```
make test
```
Test Cases are located in : C_Compiler/tests/lexer

To test a single file :
```
cd C_COMPILER/src/lexer
flex lexer.l
g++ -std=c++17 lex.yy.c Token.cpp -I. -o lexer
```
```
./lexer <INPUT_FILE_PATH>
```
## Reset
```
make clean
```
Removes any compiled files

## Keywords (40)
`auto` `break` `case` `char` `class` `const` `continue` `default` `delete`
`do` `double` `else` `enum` `extern` `float` `for` `goto` `if` `inline`
`int` `long` `new` `private` `protected` `public` `register` `restrict`
`return` `short` `signed` `sizeof` `static` `struct` `switch` `this`
`typedef` `union` `unsigned` `void` `volatile` `while` `bool`

Not implemented: `_Complex` `_Imaginary` `_Alignas` `_Alignof`
`_Atomic` `_Generic` `_Noreturn` `_Static_assert` `_Thread_local`

## Identifiers
- `[a-zA-Z_][a-zA-Z0-9_]*`
- No length limit
- ASCII only, no Unicode / `\uXXXX`

## Literals
**Integer**
- Decimal: `[0-9]+`
- Hex: `0[xX][0-9a-fA-F]+`
- No octal (`010` read as decimal)
- No binary (`0b...`)
- No suffixes (`u` `U` `l` `L` `ll` `LL`)
- No digit separators

**Float**
- `[0-9]*.[0-9]+([eE][+-]?[0-9]+)?[fFlL]?`
- `[0-9]+.([eE][+-]?[0-9]+)?[fFlL]?`
- `[0-9]+[eE][+-]?[0-9]+[fFlL]?`
- No hex float (`0x1.8p3`)

**Char**
- Single char, backslash escape, or `\xHH` hex escape
- No octal escape (`'\101'`) — falls into error rule
- Multi-char literal (`'ab'`) - error
- Unterminated - error

**String**
- `"..."`, escapes allowed, no embedded newline
- No line continuation (`\` + newline)
- No adjacent string concatenation
- No wide/prefixed strings (`L""` `u8""` `u""` `U""`)
- Unterminated - error

## Comments
- `// ...`
- `/* ... */` (not nestable)
- Unterminated block comment - error

## Operators
- Arithmetic: `+` `-` `*` `/` `%`
- Assignment: `=` `+=` `-=` `*=` `/=` `%=` `&=` `|=` `^=` `<<=` `>>=`
- Comparison: `==` `!=` `<` `<=` `>` `>=`
- Logical: `&&` `||` `!`
- Bitwise: `&` `|` `^` `~`
- Inc/Dec: `++` `--`
- Shift: `<<` `>>`
- Other: `->`

## Punctuators
`;` `,` `.` `:` `?` `(` `)` `[` `]` `{` `}`
- No `...` (ellipsis) token — lexes as three `.`

## Whitespace
- `[ \t\n\r]+` ignored
- Line numbers tracked (`yylineno`)
- Column numbers tracked internally, not printed

## Error Handling
- Recovers, does not halt, prints to `stderr`
- Format: `LEXICAL ERROR: <message> at line <n>`
- Dedicated error rules:
  - Unterminated block comment
  - Unterminated string literal
  - Multi-character constant
  - Unterminated character literal
  - Unrecognized character (catch-all)
- `Token`/`TokenType::Unknown`/`EndOfFile` defined but unused — `main()` calls raw `yylex()`, no token stream returned

## Not in Scope
- Preprocessor directives (`#include`, `#define`, macros)
- Trigraphs/digraphs
- Line splicing (`\` + newline)
- Symbol table
- Wide chars/strings, UTF-8 literals, Unicode identifiers
- Token stream handoff to parser
- CLI flags beyond single input filename

## Test Files (`tests/lexer/`)
- `test1_basicToken.c` — identifiers, basic keywords, literals, `=` `;`
- `test2_Comments.c` — `//`, `/* */`, back-to-back block comments
- `test3_Literals.c` — decimal, hex, float, exponent float, char escapes, string
- `test4_Keywords.c` — `class` `this` `new` `delete` `->`
- `test5_errors.c` — unknown symbols, multi-char literal, unterminated char/string/commentsss
