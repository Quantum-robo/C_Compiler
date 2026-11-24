// Regressions: strtod returns a double, a struct / class / union definition followed by variables,
// and implicit conversion to bool (assignment, initialization, argument, return, compound assignment)
#include <stdio.h>
#include <stdlib.h>

struct Pair { int a; int b; } origin = {3, 4}, *where = &origin;
union Cell { int whole; char part; } cell;
class Tally { public: int n; } tally;

bool asBool(int x) { return x; }
int twice(bool b) { return b + b; }
struct Flags { bool on; int n; };

int main() {
    char *end;
    double v = strtod("2.5tail", &end);
    double w = strtod("  -1.5e2xyz", &end);
    printf("strtod %.2f %.2f %s %.2f\n", v, w, end, strtod("0.25", 0) * 4);

    struct Local { int v; } one, many[2];
    one.v = 5;
    many[1].v = 6;
    cell.whole = 0;
    tally.n = 9;
    printf("decl %d %d %d %d %d %d %d\n", origin.a + origin.b, where->b, one.v, many[1].v,
           (int)sizeof(cell), cell.whole, tally.n);

    bool b1 = 7;
    bool b2 = -3;
    bool b3 = 0;
    bool b4 = 0.5;
    bool b5 = where;
    bool b6;
    b6 = 42;
    struct Flags f;
    f.on = 12;
    bool arr[2];
    arr[0] = 2;
    arr[1] = 0;
    bool *pb = &b3;
    *pb = 55;
    bool c = 1;
    c += 1;
    printf("bool %d %d %d %d %d %d %d %d %d %d %d %d %d\n", b1, b2, b3, b4, b5, b6, f.on, arr[0], arr[1], c,
           asBool(9), twice(3), asBool(0));
    return 0;
}
