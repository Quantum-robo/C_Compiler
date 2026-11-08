// Class edge cases written C++ style (no "class" keyword on declarations): composition, returning this and
// references, objects by value / by reference, arrays of objects, protected access inside the class
class Point {
public:
    int x;
    int y;
    void set(int a, int b) { x = a; y = b; }
    int sum() { return x + y; }
};

class Rect {
public:
    Point lo;
    Point hi;
    int tags[3];

    Rect *place(int x1, int y1, int x2, int y2) {
        lo.set(x1, y1);
        hi.set(x2, y2);
        return this;
    }
    int area() { return (hi.x - lo.x) * (hi.y - lo.y); }
    int &tag(int i) { return tags[i]; }
    Point &corner() { return hi; }
};

class Counter {
protected:
    int value;
private:
    int steps;
public:
    void reset() { value = 0; steps = 0; }
    Counter &add(int n) {
        value += n;
        steps++;
        return *this;
    }
    int get() { return value; }
    int count() { return steps; }
    int same_value(Counter *o) { return value == o->value; }
    void copy_from(Counter &o) {
        value = o.value;
        steps = o.steps;
    }
};

int manhattan(Point p) { return p.x + p.y; }

void shift(Point &p, int d) {
    p.x += d;
    p.y += d;
}

Point mid(Point a, Point b) {
    Point m;
    m.set((a.x + b.x) / 2, (a.y + b.y) / 2);
    return m;
}

int main() {
    Rect r;
    r.place(1, 2, 6, 9)->tags[0] = 7;
    printf("%d %d\n", r.area(), r.tags[0]);
    r.tag(1) = 8;
    r.tag(2) = 1;
    r.tag(2) += 3;
    printf("%d %d %d\n", r.tags[0], r.tags[1], r.tags[2]);
    r.corner().x = 10;
    printf("%d %d\n", r.hi.x, r.area());

    Point a;
    a.set(3, 4);
    printf("%d\n", manhattan(a));
    shift(a, 10);
    printf("%d %d\n", a.x, a.y);
    Point m = mid(a, r.lo);
    printf("%d %d\n", m.x, m.y);
    Point copy = a;
    copy.x = 0;
    printf("%d %d\n", a.x, copy.x);

    Counter c;
    c.reset();
    c.add(5).add(6).add(7);
    printf("%d %d\n", c.get(), c.count());
    Counter d;
    d.copy_from(c);
    printf("%d %d %d\n", d.get(), d.count(), d.same_value(&c));
    d.add(1);
    printf("%d %d\n", d.same_value(&c), d.get());

    Point pts[3];
    int i;
    for (i = 0; i < 3; i++) pts[i].set(i, i * 10);
    Point *pp = &pts[1];
    printf("%d %d\n", pp->sum(), pts[2].sum());
    printf("%d %d\n", sizeof(Point), sizeof(Rect));

    Rect *heap = new Rect;
    heap->place(0, 0, 4, 5);
    printf("%d\n", heap->area());
    delete heap;
    return 0;
}
