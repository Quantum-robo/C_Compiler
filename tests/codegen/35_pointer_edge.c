// Pointer edge cases: multi-level pointers, function pointers, void*, pointer arithmetic on different types
struct Pair {
    int a;
    int b;
};

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

int apply(int (*f)(int, int), int x, int y) { return f(x, y); }

void set3(int ***p) { ***p = 33; }

void swap_ptrs(int **a, int **b) {
    int *t = *a;
    *a = *b;
    *b = t;
}

int *find_max(int *a, int n) {
    int *best = a;
    int i;
    for (i = 1; i < n; i++)
        if (a[i] > *best) best = &a[i];
    return best;
}

int main() {
    int x = 1;
    int *p = &x;
    int **pp = &p;
    int ***ppp = &pp;
    ***ppp = 5;
    printf("%d %d %d\n", x, **pp, ***ppp);
    set3(ppp);
    printf("%d\n", x);

    int a = 10, b = 20;
    int *pa = &a, *pb = &b;
    swap_ptrs(&pa, &pb);
    printf("%d %d\n", *pa, *pb);

    int (*op)(int, int) = add;
    printf("%d\n", op(3, 4));
    op = mul;
    printf("%d\n", op(3, 4));
    printf("%d %d %d\n", apply(add, 8, 2), apply(sub, 8, 2), apply(mul, 8, 2));
    int (*table[3])(int, int);
    table[0] = add;
    table[1] = sub;
    table[2] = mul;
    int i;
    for (i = 0; i < 3; i++) printf("%d ", table[i](6, 3));
    printf("\n");

    int arr[5] = {3, 9, 2, 9, 4};
    int *m = find_max(arr, 5);
    printf("%d %d\n", *m, m - arr);

    struct Pair pairs[3] = {{1, 2}, {3, 4}, {5, 6}};
    struct Pair *sp = pairs;
    sp += 2;
    printf("%d %d\n", sp->a, (sp - 1)->b);
    char text[] = "pointer";
    char *cp = text + 3;
    printf("%c %d %s\n", *cp, cp - text, cp);
    int *end = arr + 5;
    int sum = 0;
    for (p = arr; p < end; p++) sum += *p;
    printf("%d\n", sum);

    void *v = &x;
    int *back = v;
    *back = 77;
    printf("%d\n", x);
    int *nothing = 0;
    printf("%d %d\n", nothing == 0, p != 0);

    char **words = new char*[3];
    words[0] = "alpha";
    words[1] = "beta";
    words[2] = "gamma";
    for (i = 0; i < 3; i++) printf("%s:%d ", words[i], strlen(words[i]));
    printf("\n");
    delete[] words;
    return 0;
}
