// Feature: structures (members, nesting, pointers, arrays of structs, by-value pass / return / assignment)
struct Point {
    int x;
    int y;
};

struct Rect {
    struct Point lo;
    struct Point hi;
    char tag;
};

struct Mixed {
    char c;
    int i;
    char d;
    short s;
};

int area(struct Rect r) {
    return (r.hi.x - r.lo.x) * (r.hi.y - r.lo.y);
}

struct Point make(int x, int y) {
    struct Point p;
    p.x = x;
    p.y = y;
    return p;
}

void move(struct Point *p, int dx, int dy) {
    p->x += dx;
    p->y += dy;
}

int main() {
    struct Point a;
    a.x = 3;
    a.y = 4;
    struct Point b = a;
    b.x = 30;
    printf("%d,%d %d,%d\n", a.x, a.y, b.x, b.y);

    struct Rect r;
    r.lo.x = 1; r.lo.y = 2;
    r.hi.x = 6; r.hi.y = 9;
    r.tag = 'R';
    printf("area=%d tag=%c\n", area(r), r.tag);

    struct Point c = make(7, 8);
    printf("%d %d\n", c.x, c.y);
    move(&c, 1, -1);
    printf("%d %d\n", c.x, c.y);

    struct Point pts[3];
    int i;
    for (i = 0; i < 3; i++) {
        pts[i].x = i;
        pts[i].y = i * i;
    }
    struct Point *pp = pts;
    pp++;
    printf("%d %d\n", pp->x, pp->y);
    pts[2] = pts[1];
    printf("%d %d\n", pts[2].x, pts[2].y);
    int total = 0;
    for (i = 0; i < 3; i++) total += pts[i].x + pts[i].y;
    printf("total=%d\n", total);

    struct Point init = {5, 6};
    printf("%d %d\n", init.x, init.y);
    struct Mixed m;
    m.c = 'x'; m.i = 123456; m.d = 'y'; m.s = 77;
    printf("%c %d %c %d size=%d\n", m.c, m.i, m.d, m.s, sizeof(struct Mixed));
    printf("sizes %d %d %d\n", sizeof(struct Point), sizeof(struct Rect), sizeof r);
    return 0;
}
