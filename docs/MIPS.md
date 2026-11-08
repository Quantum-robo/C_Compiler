# MIPS code generation

The code generator (`src/codegen/MipsGen.cpp`) turns the typed three-address code described in [IR.md](IR.md)
into MIPS32 assembly for the **SPIM** simulator. The runtime library (`src/codegen/Runtime.cpp`) is appended to
every output, so one `.s` file is a complete program.

```bash
./compiler prog.c              # compile prog.c; writes the assembly (prog.s); add --tac prog.tac for the TAC
spim -file prog.s              # run it  (sudo apt install spim)
```

`QtSpim` and MARS can load the same file. A program's `main` return value is its exit code. `int main(int argc, char **argv)` receives SPIM's `argc` / `argv`
(`spim -file prog.s arg1 arg2`).

## Scheme

The generator is deliberately simple and always correct; it does not optimise.

* Every variable and every temporary has a slot in the stack frame (`$fp`-relative); globals and static locals live in
  the data segment as `G_<name>`.
* Each TAC instruction loads its operands into `$t0`/`$t1`/`$t2`, computes, and stores the result back.
* `char` and `short` variables are accessed with `lb`/`lh`/`sb`/`sh`; everything else with `lw`/`sw`. Struct objects are
  copied word by word (a loop above 64 bytes). Temporaries are always one word (or a whole struct).
* Frame offsets that do not fit in 16 bits (a function with a big local array) go through `$t9`.

### Frame layout

```text
   higher addresses
   +---------------------+
   | argument n-1        |
   | ...                 |
   | argument 0          |  8($fp)   (the first argument is at the lowest address)
   | hidden result ptr   |  only for a function returning a struct by value (before argument 0)
   +---------------------+
   | saved $ra           |  4($fp)
   | saved $fp           |  0($fp)   <- $fp
   +---------------------+
   | locals, temporaries |  -4($fp), -8($fp), ...
   +---------------------+  <- $sp
```

### Calling convention

* The caller pushes the arguments right to left (a method's object pointer last), executes `jal`, then pops them.
* A struct argument is pushed as a copy of the whole object. A reference parameter receives an address.
* A function returning a struct gets one extra argument, pushed last: the address where the result is stored.
* All other results are returned in `$v0`; a function returning `float` / `double` returns a double in `$f0`.
* A `double` argument takes two words (low word at the lower address), a `float` argument one.
* `__global_init` runs the initializers of globals and static locals; the entry point `main` calls it, then `F_main`,
  then exits with `main`'s result (syscall 17).

### Floating point

Every floating value is computed as a double in `$f0` / `$f2` (`add.d`, `sub.d`, `mul.d`, `div.d`, `neg.d`,
`c.lt.d` / `c.le.d` / `c.eq.d` with `bc1t`). A `float` variable is stored in single format and converted with
`cvt.d.s` / `cvt.s.d` when it is loaded or stored; since each operation result is rounded when it is stored into a
`float` temporary, this gives the same values as single-precision arithmetic. An `int` meeting a floating operand is
converted with `cvt.d.w`; a floating value stored into an integer variable is truncated (`trunc.w.d`), into a `bool`
it becomes 0 or 1. Doubles are moved as two `lwc1` / `swc1`, so they need no 8-byte alignment. Literals are kept in
the data segment (`D_0`, `D_1`, ...). The code generator picks floating or integer instructions from the operands
(see [IR.md](IR.md)).

### Names

| In the assembly       | Meaning                                           |
| --------------------- | ------------------------------------------------- |
| `F_name`, `F_Class__method` | a function / a method                        |
| `G_name`, `G_func_n`  | a global / a static local                          |
| `L12`                 | a compiler label                                   |
| `U_main_retry`        | the source label `retry` in `main`                 |
| `S_3`                 | the fourth string literal                          |
| `D_2`                 | the third floating-point literal (two data words)  |
| `RF_printf`, `RG_x`   | runtime library functions and globals (prefix R)   |
| `rt_sys_exit`, ...    | hand-written system call stubs                     |

## Runtime library

The runtime library is written in the C subset this compiler accepts (`kRuntimeSource` in `src/codegen/Runtime.cpp`).
When a `.s` file is produced, the compiler first compiles the program, then compiles the library with the same
machinery, and appends **only the library functions the program uses** (directly or through each other). The library
has its own label prefix (`RF_printf`, `RG_rt_seed`, ...), so a program can define its own `rt_pad`, `abs` or `strlen`
without a clash: calls in the program resolve to the program's function first.

Only five small pieces are hand-written assembly (`rt_sys_putchar`, `rt_sys_readline`, `rt_sys_sbrk`, `rt_sys_exit`
and `rt_sys_sqrt`, which is one `sqrt.d`). A variadic function such as `printf(char *fmt)` is declared with its first parameter only and finds the
other arguments on the stack right behind it (`&fmt + 4`).

| Header        | Functions provided                                                                                       |
| ------------- | -------------------------------------------------------------------------------------------------------- |
| `stdio.h`     | `printf` (`%d %i %u %x %X %o %c %s %p %f %e %E %g %G %%`; flags `- 0 + space`; width, `*`, `.precision`; `l`/`h` ignored), `scanf` (`%d %i %u %x %o %c %s %f %e %g %%`; `%f` stores a `float`, `%lf` a `double`; width, `%*d`, `h`; returns the count or -1 at end of input), `sprintf`, `snprintf` (same conversions as `printf`), `putchar`, `puts`, `getchar`, `gets` |
| `stdlib.h`    | `malloc`, `calloc`, `realloc`, `free` (free list kept sorted by address, neighbours merged), `abs`, `labs`, `atoi`, `atof`, `strtod`, `rand`, `srand`, `exit` |
| `math.h`      | `sqrt`, `pow`, `fabs`, `floor`, `ceil`, `round`, `fmod`, `exp`, `log`, `log10`, `sin`, `cos`, `tan`      |
| `string.h`    | `strlen`, `strcpy`, `strncpy`, `strcat`, `strncat`, `strcmp`, `strncmp`, `strchr`, `strrchr`, `strstr`, `memset`, `memcpy`, `memmove`, `memcmp` |
| `ctype.h`     | `isdigit`, `isalpha`, `isalnum`, `isupper`, `islower`, `isspace`, `isprint`, `ispunct`, `isxdigit`, `toupper`, `tolower` |

`new` / `delete` compile to `malloc` / `free`. A call to any other library function (`fopen`, `sscanf`, `atan`, ...) is a
`CODE GENERATION ERROR`. Input is read a line at a time (SPIM reports end of input only through `read_string`).

## Limits

* No file I/O or `sscanf`; no `long double`, `unsigned` arithmetic, `asin` / `acos` / `atan` / `atan2`.
* The math functions (`sqrt`, `pow`, `exp`, `log`, `sin`, `cos`, ...) are series approximations, accurate to about 10
  decimals; `sqrt` uses the `sqrt.d` instruction. Printing with `%f` is exact for ordinary values (round to nearest,
  ties to even, as glibc does) but loses digits beyond 15-17 significant digits.
* `rand` is a simple linear congruential generator, not the C library's sequence.
* Function arguments are evaluated left to right (C leaves the order unspecified).
* `cond ? a : b` is never an lvalue, so `return flag ? a : b;` is not allowed in a function returning a reference.
