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
* The result is in `$v0`; a function returning `T&` returns an address.
* `printf`, `scanf` and any other undeclared function are called like any other; the callee reads its arguments
  from the stack. The runtime library provides them (see [MIPS.md](MIPS.md)); the IR relies on `malloc`, `free` and
  `strcpy` for `new`, `delete` and string initialisers.

## 7. Known limits

* `float` and `double` are typed correctly (sizes 4 and 8) but the arithmetic is still integer-only; floating point is
  a stretch goal.
* Pointer arithmetic on a pointer to an array (`int (*p)[3]`) is approximate (a multi-dimensional array name is handled).
* An array size must be an integer constant (a literal, an enumerator, or a `const int` with a constant value).
* A function pointer is an untyped 4-byte address.
