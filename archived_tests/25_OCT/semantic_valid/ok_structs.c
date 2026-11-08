struct Point { int x; int y; };
struct Line { struct Point a; struct Point b; };
union Num { int i; float f; };
int main() {
    struct Point p1;
    struct Point p2;
    struct Line ln;
    struct Point pts[3];
    struct Point *pp = &p1;
    union Num u;
    p1.x = 1;
    p1.y = 2;
    p2 = p1;
    ln.a = p1;
    ln.b.x = pp->x + p2.y;
    pts[1] = p2;
    pts[2].x = ln.b.x;
    pp = &pts[1];
    pp->y = pts[2].x;
    u.i = 3;
    u.f = 2.5;
    return u.i + pp->y;
}
