// Feature: static variables (persist between calls, one per function, initialised once, static globals)
int counter() {
    static int n = 0;
    n++;
    return n;
}

int other() {
    static int n = 100;
    n += 10;
    return n;
}

int fibonacci_next() {
    static int a = 0;
    static int b = 1;
    int t = a;
    a = b;
    b = t + b;
    return t;
}

static int hidden = 7;

int main() {
    int i;
    for (i = 0; i < 4; i++) printf("%d ", counter());
    printf("\n");
    int o1 = other();
    int o2 = other();
    printf("%d %d %d\n", o1, o2, counter());
    for (i = 0; i < 8; i++) printf("%d ", fibonacci_next());
    printf("\n");
    hidden++;
    printf("%d\n", hidden);
    static int local_static = 5;
    local_static *= 2;
    printf("%d\n", local_static);
    return 0;
}
