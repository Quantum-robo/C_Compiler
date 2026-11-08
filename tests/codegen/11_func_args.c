// Feature: function calls with arguments (many arguments, nesting, char arguments, void functions)
int add(int a, int b) { return a + b; }
int mul3(int a, int b, int c) { return a * b * c; }
int pick(int a, int b, int c, int d, int e, int f, int g) {
    return a + 2 * b + 3 * c + 4 * d + 5 * e + 6 * f + 7 * g;
}
char next(char c) { return c + 1; }
void show(int a, char c, int b) { printf("show %d %c %d\n", a, c, b); }
int max(int a, int b) { return a > b ? a : b; }
int apply_twice(int x) { return add(add(x, 1), add(x, 2)); }
void noargs() { printf("no args\n"); }
void modify(int v) { v = v * 100; printf("inside %d\n", v); }

int main() {
    printf("%d\n", add(2, 3));
    printf("%d\n", mul3(2, 3, 4));
    printf("%d\n", pick(1, 2, 3, 4, 5, 6, 7));
    printf("%c\n", next('a'));
    show(1, 'z', 3);
    printf("%d\n", max(add(1, 2), mul3(1, 2, 2)));
    printf("%d\n", apply_twice(10));
    noargs();
    int v = 5;
    modify(v);
    printf("outside %d\n", v);
    printf("%d\n", add(add(add(1, 2), add(3, 4)), add(add(5, 6), 7)));
    return 0;
}
