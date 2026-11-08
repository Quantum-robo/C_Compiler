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
* All other results are returned in `$v0`.
* `__global_init` runs the initializers of globals and static locals; the entry point `main` calls it, then `F_main`,
  then exits with `main`'s result (syscall 17).

### Names

| In the assembly       | Meaning                                           |
| --------------------- | ------------------------------------------------- |
| `F_name`, `F_Class__method` | a function / a method                        |
| `G_name`, `G_func_n`  | a global / a static local                          |
| `L12`                 | a compiler label                                   |
| `U_main_retry`        | the source label `retry` in `main`                 |
| `S_3`                 | the fourth string literal                          |
| `RF_printf`, `RG_x`   | runtime library functions and globals (prefix R)   |
| `rt_sys_exit`, ...    | hand-written system call stubs                     |

## Runtime library

The runtime library is written in the C subset this compiler accepts (`kRuntimeSource` in `src/codegen/Runtime.cpp`).
When a `.s` file is produced, the compiler first compiles the program, then compiles the library with the same
machinery, and appends **only the library functions the program uses** (directly or through each other). The library
has its own label prefix (`RF_printf`, `RG_rt_seed`, ...), so a program can define its own `rt_pad`, `abs` or `strlen`
without a clash: calls in the program resolve to the program's function first.

Only four operating-system calls are hand-written assembly (`rt_sys_putchar`, `rt_sys_readline`, `rt_sys_sbrk`,
`rt_sys_exit`). A variadic function such as `printf(char *fmt)` is declared with its first parameter only and finds the
other arguments on the stack right behind it (`&fmt + 4`).

| Header        | Functions provided                                                                                       |
| ------------- | -------------------------------------------------------------------------------------------------------- |
| `stdio.h`     | `printf` (`%d %i %u %x %X %o %c %s %p %%`; flags `- 0 + space`; width, `*`, `.precision` for `%s`; `l`/`h` ignored), `scanf` (`%d %i %u %x %o %c %s %%`; width, `%*d`, `h`; returns the count or -1 at end of input), `sprintf`, `snprintf` (same conversions as `printf`), `putchar`, `puts`, `getchar`, `gets` |
| `stdlib.h`    | `malloc`, `calloc`, `realloc`, `free` (free list kept sorted by address, neighbours merged), `abs`, `labs`, `atoi`, `rand`, `srand`, `exit` |
| `string.h`    | `strlen`, `strcpy`, `strncpy`, `strcat`, `strncat`, `strcmp`, `strncmp`, `strchr`, `strrchr`, `strstr`, `memset`, `memcpy`, `memmove`, `memcmp` |
| `ctype.h`     | `isdigit`, `isalpha`, `isalnum`, `isupper`, `islower`, `isspace`, `isprint`, `ispunct`, `isxdigit`, `toupper`, `tolower` |

`new` / `delete` compile to `malloc` / `free`. A call to any other library function (`fopen`, `sscanf`, `sqrt`, ...) is a
`CODE GENERATION ERROR`. Input is read a line at a time (SPIM reports end of input only through `read_string`).

## Limits

* No floating point yet (`float` / `double` literals are rejected with a clear message); no file I/O, `sscanf` or math library.
* `rand` is a simple linear congruential generator, not the C library's sequence.
* Function arguments are evaluated left to right (C leaves the order unspecified).
* `cond ? a : b` is never an lvalue, so `return flag ? a : b;` is not allowed in a function returning a reference.
