// Feature: class and object (members, methods, methods calling methods, objects through pointers)
class Counter {
public:
    int value;
    int step;

    void init(int start, int s) {
        value = start;
        step = s;
    }
    void tick() { value = value + step; }
    void tick_n(int n) {
        int i;
        for (i = 0; i < n; i++) tick();
    }
    int get() { return value; }
};

class Vec {
public:
    int x;
    int y;
    int dot(class Vec *o) { return x * o->x + y * o->y; }
    void add(class Vec *o) {
        x += o->x;
        y += o->y;
    }
    int len2() { return dot(this); }
};

int main() {
    class Counter c;
    c.init(10, 3);
    c.tick();
    c.tick_n(4);
    printf("value=%d\n", c.get());

    class Counter d;
    d.init(0, 1);
    class Counter *pd = &d;
    pd->tick_n(5);
    printf("d=%d c=%d\n", pd->get(), c.get());
    pd->value = 100;
    printf("%d\n", d.value);

    class Vec a, b;
    a.x = 1; a.y = 2;
    b.x = 3; b.y = 4;
    printf("dot=%d\n", a.dot(&b));
    a.add(&b);
    printf("%d %d len2=%d\n", a.x, a.y, a.len2());

    class Counter list[3];
    int i;
    for (i = 0; i < 3; i++) {
        list[i].init(i * 10, i + 1);
        list[i].tick();
    }
    for (i = 0; i < 3; i++) printf("%d ", list[i].get());
    printf("\n");
    printf("sizeof Counter=%d\n", sizeof(class Counter));
    return 0;
}
