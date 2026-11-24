# Intermediate representation (typed three-address code)

The parser reads the program twice and produces three-address code (TAC) plus tables describing every
type, variable and temporary. The MIPS code generator works from these and nothing else.

```bash
./compiler --check --tac prog.tac prog.c            # write the TAC (no assembly)
./compiler --check --tables --print-tac prog.c      # print it with the layouts and symbol tables
```

## 1. Two passes

A method may use members that are declared later in its class, and may use the size of its own class
(`new Node`, `sizeof(Node)`). The offsets are not known when the body is parsed, so the program is read twice:

1. **Pass 1** only collects the layout of every struct, class and union. Diagnostics are discarded.
2. **Pass 2** compiles with every layout known, reports diagnostics and generates the IR.

## 2. Operands

Every operand is one of:

| Operand            | Example                          | Meaning                                                              |
| ------------------ | -------------------------------- | -------------------------------------------------------------------- |
| integer literal    | `42`, `-3`, `0x1F`, `'a'`, `'\n'`| decimal, hex or character literal                                    |
| string literal     | `"hello\n"`                      | address of the string in the data segment                            |
| temporary          | `t7`                             | one word, or a whole object when its type is a struct/class/union    |
| variable           | `sum`, `x.2`, `counter.n`, `this`| IR name, unique inside its function (see section 5)                  |

Field access, indexing, pointer scaling, `sizeof`, `new` and `delete` never appear inside an operand: they have
been lowered to the instructions below.

## 3. Instructions

| Printed form                         | Meaning                                                                          |
| ------------------------------------ | -------------------------------------------------------------------------------- |
| `x = y`                              | copy; word, byte, or the whole object for a struct (sized by the variable)       |
| `t = a + b` (and `- * / % << >> & \| ^ == != < <= > >=`) | integer operation on words. `+` is a plain add: pointer arithmetic is already scaled |
| `t = -a`, `t = ~a`, `t = !a`         | unary operations                                                                 |
| `t = &x`                             | address of a variable. An array name is already its own address                  |
| `t = *p`, `t = *(p + 8)`             | load `sz` bytes from `p + off`                                                   |
| `*p = v`, `*(p + 8) = v`             | store `sz` bytes to `p + off`                                                    |
| `t = a[i]`, `a[i] = v`, `t = &a[i]`  | element `i` of `a` (an element index, not bytes); `sz` is the element size       |
| `Zero x :N`                          | clear the N bytes of local `x` (emitted before brace initializers)               |
| `PushParam v`                        | push an argument (arguments are pushed right to left)                            |
| `t = LCall f` / `LCall f`            | call; `PopParams n` follows when n arguments were pushed                         |
| `Return v` / `Return`                | return                                                                           |
| `L1:`, `Goto L1`, `If t Goto L1`, `IfFalse t Goto L1` | control flow                                                    |
| `f:` + `BeginFunc` ... `EndFunc`     | function body (methods are `Class::method`)                                      |

A trailing `:N` on a memory instruction (`t = *p :1`) is its size in bytes. Without it the size is 4 (one word).
`sz` and the pointer offset `off` are fields of `TACInstruction`; they are what the code generator reads.

### Lowering done by the parser

| Source                | IR                                                                                 |
| --------------------- | ---------------------------------------------------------------------------------- |
| `p + n`, `p - n`      | `t = n * sizeof(*p)`, then `p + t`                                                 |
| `p++`, `p += n`       | step of `sizeof(*p)` bytes                                                         |
| `p - q`               | `(p - q) / sizeof(*p)`                                                             |
| `s.f`, `p->f`         | address plus constant field offset: `*(&s + 8)`, `*(p + 8)`                        |
| `a[i][j]` (`T a[R][C]`) | one element index `i*C + j`                                                      |
| `a[i]` of a struct array | `&a[i] :12`, then a field offset                                                |
| `sizeof x`, `sizeof(T)` | an integer literal                                                               |
| `new T`, `new T[n]`   | `PushParam size` (or `n * size`), `t = LCall malloc`                               |
| `delete p`, `delete[] p` | `PushParam p`, `LCall free`                                                     |
| `char s[] = "ab"`, a `"ab"` row of a char array | `Zero`, then `PushParam "ab"`, `PushParam &s`, `LCall strcpy` |
| `r = x` (reference)   | the reference variable holds an address; every read or write goes through `*r`    |
| constant arithmetic   | folded (`2 * 3` is `6`)                                                            |
| source label / `goto` | `label` / `goto` instructions carry `"user"` in `arg1`, so they can be told apart from `L1`, `L2`... |

## 4. Layout (MIPS32)

| Type                         | Size | Alignment |
| ---------------------------- | ---- | --------- |
| `char`, `bool`               | 1    | 1         |
| `short`                      | 2    | 2         |
| `int`, `long`, `float`, any pointer | 4 | 4     |
| `double`                     | 8    | 8         |
| `struct` / `class`           | fields in order, each at its own alignment; total rounded up to the largest alignment | |
| `union`                      | every field at offset 0; size of the largest field, rounded up | |

An empty struct has size 1. Methods take no space. `src/semantic/Layout.{hpp,cpp}` computes all of this.

## 5. Tables

`TACGenerator` (see `src/ir/TAC.hpp`) holds:

* **`globalVars()`** - globals and static locals (`counter.n`), with type, dimensions and `isStatic`.
* **`functions()`** - one `FuncInfo` per function: parameters in order (a method's `this` is first), locals, the type of
  every temporary, and the return type. Temporaries that are structs get a slot of the struct's size; every other
  temporary is one word.
* **`initInfo()` / `initCode()`** - the temporaries and instructions of `_global_init`: the initializers of globals
  and static locals. The program must run it once before `main`.
* **`finiInfo()` / `finiCode()`** - `_global_fini`: the destructors of global objects, to run after `main`.
* **Constructors and destructors** are ordinary methods named `Class::Class#N` (N = number of parameters) and
  `Class::~Class`; the parser inserts the calls (at declarations, at the end of scopes, in `new` / `delete`).

Every local has an IR name that is unique inside its function: a second variable called `i` becomes `i.2`, and a
local that shadows a global is renamed too (`x.1`). A static local is `function.name`. The code generator therefore
needs no scope information.

Variable kinds that the code generator must tell apart when it uses a name as an operand:

| Declared as                              | The name as an operand                           |
| ---------------------------------------- | ------------------------------------------------ |
| scalar, struct (local, param, global)    | its value (a struct: the object itself)          |
| array (local or global)                  | its address                                      |
| array parameter (`int a[]`, `int m[][3]`)| a pointer: its value is the address              |
| reference (`isRef`)                      | the address it holds (the parser inserts the `*`)|

## 6. Calling convention the IR expects

* Arguments are pushed right to left, so the first argument is on top of the stack at the call.
* For a method call the object address is pushed last: `this` is parameter 0.
* A reference parameter receives an address. A struct parameter is pushed as a copy of the whole object.
* The result is in `$v0` (in `$f0`, as a double, for a function returning `float` / `double`); a function returning
  `T&` returns an address. A `double` argument takes two words, a `float` argument one.
* `printf`, `scanf` and any other undeclared function are called like any other; the callee reads its arguments
  from the stack. The runtime library provides them (see [MIPS.md](MIPS.md)); the IR relies on `malloc`, `free` and
  `strcpy` for `new`, `delete` and string initialisers.

## 7. Known limits

* Floating point: operators carry no type; the code generator looks at the operands (a floating literal, or a variable
  or temporary of type `float` / `double`) and chooses floating or integer instructions, converting between `int`,
  `float` and `double` on a copy (`x = y`). The front end adds the conversion copies that cannot be seen from the
  operands: an argument is converted to its parameter type (a `float` passed to a function without a prototype, such as
  `printf`, becomes a `double`) and a cast is a copy into a temporary of the target type. A `float` in memory carries
  the size tag `:f4` (`*p = x :f4`, `a[i] :f4`), a `double` has `:8`. `unsigned` and `long double` are not supported.
* Pointer arithmetic on a pointer to an array (`int (*p)[3]`) is approximate (a multi-dimensional array name is handled).
* An array size must be an integer constant (a literal, an enumerator, or a `const int` with a constant value).
* A function pointer is an untyped 4-byte address.

## 8. Optimizer

`src/optimizer/Optimizer.{hpp,cpp}` rewrites the TAC of every function before code generation. It is on by default
(`-O0` turns it off). `--print-opt-tac` / `--opt-tac <file>` show the result; `--print-tac` / `--tac` still show the IR
as the parser produced it.

```bash
./compiler --check --print-opt-tac prog.c      # optimised TAC + a summary line of what was done
./compiler -O0 prog.c                          # assembly from the unoptimised TAC
```

All optimizations are **local**: facts are kept only inside one basic block (they are forgotten at every label and
after every jump), so no data-flow analysis across blocks is needed.

| Optimization | Example (before -> after) |
| --- | --- |
| Constant folding | `t = 10 * 4` -> `t = 40` (32-bit wrap-around; `/` and `%` truncate; division by zero is left alone) |
| Constant propagation | `b = 10; t = b * 4` -> `b = 10; t = 10 * 4` (then folded) |
| Copy propagation | `b = a; t = b + 1` -> `b = a; t = a + 1` |
| Common subexpression elimination | `t1 = w * h; ...; t3 = h * w` -> `t3 = t1` (`+ * & \| ^ == !=` are commutative; `&x` is reused too) |
| Branch on a constant | `IfFalse 1 Goto L` is removed, `IfFalse 0 Goto L` becomes `Goto L` |
| Dead code elimination | removes a result that is never read (a temporary or a local), `x = x`, code after `Goto` / `Return` that no label reaches, a jump to the next instruction and labels no jump uses; an unread call result is dropped but the call stays |

Temporaries that no instruction mentions any more get no stack slot. The pass is repeated until nothing changes.

**Which variables are optimized.** Only one-word integers and pointers that are locals, parameters or temporaries of the
function, are not references and whose address is never taken (`&x`). Their value can only change through an
assignment the optimizer sees, so a call or a store through a pointer can never invalidate what it knows. Globals,
static locals, arrays, structs, `char` / `short` / `bool` (narrower than a word: storing truncates) and
`float` / `double` are left as they are. Memory loads (`*p`, `a[i]`) are never reused, because a store may change memory.

Tests: `tests/opt/` (one program per optimization; the optimised TAC must equal `tests/opt/expected/<name>.tac`) and
`tests/codegen/49_optimizer` (run on SPIM, output compared with g++, including the cases where optimizing would be wrong).
Every end-to-end test gives the same output with and without the optimizer.
