# MIPS code generation

The code generator (`src/codegen/MipsGen.cpp`) turns the typed three-address code described in [IR.md](IR.md)
into MIPS32 assembly for the **SPIM** simulator. The runtime library (`src/codegen/Runtime.cpp`) is appended to
every output, so one `.s` file is a complete program.

```bash
./parser -S prog.s prog.c      # compile prog.c; writes the TAC (prog.tac) and the assembly (prog.s)
spim -file prog.s              # run it  (sudo apt install spim)
```

`QtSpim` and MARS can load the same file. A program's `main` return value is its exit code.

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
| `rt_printf`, ...      | runtime library functions                          |

## Runtime library

Runtime functions use the same calling convention (argument 0 at `0($sp)` on entry, result in `$v0`) and are written in
assembly. Currently provided: `printf` (`%d %c %s %%`), `malloc`, `free`, `strcpy`, `strlen`, `strcmp`, `exit`.
`new` / `delete` compile to `malloc` / `free`. A call to any other library function is a code generation error.

## Limits

* No `scanf` yet and no floating point (`float`, `double` literals are rejected with a clear message).
* `free` does nothing yet (memory is never reused).
* `printf` has no field widths or `%x`.
* Function arguments are evaluated left to right (C leaves the order unspecified).
* `cond ? a : b` is never an lvalue, so `return flag ? a : b;` is not allowed in a function returning a reference.
