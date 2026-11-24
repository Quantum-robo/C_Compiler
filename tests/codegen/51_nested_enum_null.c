// Regressions: types defined inside a struct / class body, enum values given by constant expressions (1 << 1, ~0,
// !0, |, ^), an enum definition followed by a variable, nullptr, octal literals (010 is 8), and main without a
// return statement (the exit status must be 0: see 51_nested_enum_null.status)
#include <stdio.h>

enum Mode { OFF = 0, ON = 1 << 1, ERR = ~0, MASK = (1 << 4) - 1, NOTZ = !0, BOTH = ON | 1, X3 = 6 ^ 3, OCT = 010 } mode = ON;
enum { RED, GREEN } colour = GREEN;

struct Outer {
    struct Inner { int a; char tag; } in;
    union Bits { int whole; char part[4]; } bits;
    int b;
};

class Holder {
public:
    struct Meta { int id; bool active; } meta;
    int value;
};

int octal = 010 + 1;
int folded = (1 << 3) | (0x10 >> 2) | (5 & 3) | (6 ^ 1);

int main() {
    struct Outer o;
    o.in.a = 4;
    o.in.tag = 'q';
    o.bits.whole = 0;
    o.b = 5;
    Holder h;
    h.meta.id = 7;
    h.meta.active = 9;
    h.value = 1;
    int *p = nullptr;
    int *q = 0;
    int arr[010];
    printf("enum %d %d %d %d %d %d %d %d %d %d\n", ON, ERR, MASK, NOTZ, BOTH, X3, OFF, OCT, mode, colour);
    printf("nested %d %c %d %d %d %d\n", o.in.a, o.in.tag, o.b, h.meta.id, h.meta.active, (int)sizeof(struct Outer));
    printf("null %d %d\n", p == nullptr, p == q);
    printf("octal %d %d %d %d\n", octal, 017, folded, (int)(sizeof(arr) / sizeof(arr[0])));
}
