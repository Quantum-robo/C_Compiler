// Feature: structure (definition, member access, nested struct, pointer -> access)
struct Point {
    int x;
    int y;
};

struct Rect {
    struct Point topLeft;
    struct Point bottomRight;
};

int area(struct Rect r) {
    int w = r.bottomRight.x - r.topLeft.x;
    int h = r.bottomRight.y - r.topLeft.y;
    return w * h;
}

int main() {
    struct Point p;
    p.x = 3;
    p.y = 4;
    struct Point *pp = &p;
    pp->x = pp->x + 1;
    struct Rect r;
    r.topLeft.x = 0;
    r.topLeft.y = 0;
    r.bottomRight.x = p.x;
    r.bottomRight.y = p.y;
    int a = area(r);
    return a;
}
