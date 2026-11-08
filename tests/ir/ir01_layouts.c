// IR: struct and union layout (padding, nesting, arrays inside structs) and sizeof
struct Mixed { char c; int i; char d; short s; };
struct Inner { char tag; int vals[2]; };
struct Outer { char flag; struct Inner in; struct Inner list[2]; };
union U { char c; int i; short s[3]; };

int main() {
    struct Mixed m;
    struct Outer o;
    union U u;
    m.c = 'x';
    m.i = 100;
    m.d = 'y';
    m.s = 7;
    o.flag = 1;
    o.in.tag = 2;
    o.in.vals[1] = 3;
    o.list[1].vals[0] = 4;
    u.i = 5;
    int total = sizeof(struct Mixed) + sizeof o + sizeof(union U) + sizeof(struct Inner *);
    return m.i + o.in.vals[1] + o.list[1].vals[0] + u.i + total;
}
