// Struct edge cases: nested returns, structs in unions and arrays, by-value copies, self-referential structs
struct Vec {
    int x;
    int y;
};

struct Line {
    struct Vec a;
    struct Vec b;
};

struct Shape {
    int kind;
    union {
        struct Vec point;
        int radius;
    } u;
};

struct Item {
    char name[8];
    int qty;
    struct Item *next;
};

struct Vec make(int x, int y) {
    struct Vec v;
    v.x = x;
    v.y = y;
    return v;
}

struct Vec add(struct Vec a, struct Vec b) {
    struct Vec r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    return r;
}

struct Line mkline(int x1, int y1, int x2, int y2) {
    struct Line l;
    l.a = make(x1, y1);
    l.b = make(x2, y2);
    return l;
}

int len2(struct Line l) {
    int dx = l.b.x - l.a.x;
    int dy = l.b.y - l.a.y;
    l.a.x = 12345;
    return dx * dx + dy * dy;
}

int main() {
    struct Vec p = add(make(1, 2), make(10, 20));
    printf("%d %d\n", p.x, p.y);
    printf("%d %d\n", make(7, 8).x, add(p, p).y);

    struct Line l = mkline(0, 0, 3, 4);
    printf("%d %d\n", len2(l), l.a.x);

    struct Line lines[2] = {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
    printf("%d %d %d\n", lines[0].b.y, lines[1].a.x, lines[1].b.x);
    lines[0] = lines[1];
    lines[1].a.x = 0;
    printf("%d %d\n", lines[0].a.x, lines[1].a.x);

    struct Vec pts[3] = {{1, 1}, {2, 4}, {3, 9}};
    struct Vec *q = pts;
    int i, s = 0;
    for (i = 0; i < 3; i++, q++) s += q->x * q->y;
    printf("%d\n", s);

    struct Shape sh;
    sh.kind = 1;
    sh.u.point = make(5, 6);
    printf("%d %d %d\n", sh.kind, sh.u.point.x, sh.u.point.y);
    sh.kind = 2;
    sh.u.radius = 9;
    printf("%d %d\n", sh.kind, sh.u.radius);
    printf("%d %d\n", sizeof(struct Shape), sizeof(struct Line));

    struct Item items[3];
    for (i = 0; i < 3; i++) {
        items[i].name[0] = 'a' + i;
        items[i].name[1] = 0;
        items[i].qty = (i + 1) * 5;
        items[i].next = i < 2 ? &items[i + 1] : 0;
    }
    struct Item *it = &items[0];
    int total = 0;
    while (it != 0) {
        total += it->qty;
        printf("%s ", it->name);
        it = it->next;
    }
    printf("total=%d\n", total);

    struct Vec a = make(1, 2), b = a;
    b.x = 100;
    printf("%d %d %d\n", a.x, b.x, a.x == b.x);
    return 0;
}
