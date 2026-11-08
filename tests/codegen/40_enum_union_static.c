// enum (named, anonymous, negative, as index and in switch), union, and static edge cases
enum Color { RED, GREEN = 5, BLUE };
enum { LOW = -2, MID, HIGH = 10, TOP };

enum Op { ADD, SUB, MUL };

union Word {
    int i;
    char b[4];
};

union Mixed {
    struct {
        short lo;
        short hi;
    } halves;
    int whole;
};

int names_len[3] = {3, 5, 4};

int eval(enum Op op, int a, int b) {
    switch (op) {
        case ADD: return a + b;
        case SUB: return a - b;
        case MUL: return a * b;
    }
    return 0;
}

Color next(Color c) {
    if (c == RED) return GREEN;
    if (c == GREEN) return BLUE;
    return RED;
}

int calls() {
    static int n;
    static int hist[3];
    hist[n % 3] = n;
    n++;
    return hist[0] + hist[1] + hist[2];
}

int *table() {
    static int t[3] = {10, 20, 30};
    t[0]++;
    return t;
}

int fact_static(int n) {
    static int depth = 0;
    int r;
    depth++;
    r = n <= 1 ? depth : n * fact_static(n - 1);
    return r;
}

int main() {
    Color c = RED;
    int i;
    for (i = 0; i < 4; i++) {
        printf("%d ", c);
        c = next(c);
    }
    printf("\n");
    printf("%d %d %d %d\n", LOW, MID, HIGH, TOP);
    printf("%d %d %d\n", eval(ADD, 6, 3), eval(SUB, 6, 3), eval(MUL, 6, 3));
    printf("%d\n", names_len[MUL] + names_len[ADD]);
    enum Op ops[3] = {MUL, ADD, SUB};
    int acc = 10;
    for (i = 0; i < 3; i++) acc = eval(ops[i], acc, 2);
    printf("%d\n", acc);
    printf("%d %d\n", BLUE - GREEN, GREEN > RED);

    union Word w;
    w.i = 0;
    w.b[0] = 1;
    w.b[1] = 2;
    printf("%d %d %d\n", w.i, w.b[1], sizeof(union Word));
    union Mixed m;
    m.whole = 0;
    m.halves.lo = 5;
    m.halves.hi = 1;
    printf("%d %d %d\n", m.whole, m.halves.hi, sizeof(union Mixed));

    for (i = 0; i < 5; i++) printf("%d ", calls());
    printf("\n");
    table();
    int *t = table();
    printf("%d %d %d\n", t[0], t[1], t[2]);
    printf("%d\n", fact_static(4));
    return 0;
}
