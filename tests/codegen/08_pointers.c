// Feature: pointers (address-of, dereference, arithmetic, pointer to pointer, swap, array walking)
void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int strlen2(char *s) {
    char *p = s;
    while (*p) p++;
    return p - s;
}

void incr_all(int *p, int n) {
    int i;
    for (i = 0; i < n; i++) p[i]++;
}

int main() {
    int x = 5, y = 9;
    int *px = &x;
    int *py = &y;
    printf("%d %d\n", *px, *py);
    swap(px, py);
    printf("%d %d\n", x, y);
    *px = *px * 10;
    printf("%d\n", x);

    int **pp = &px;
    **pp = 77;
    printf("%d %d\n", x, **pp);

    int arr[5] = {1, 2, 3, 4, 5};
    int *p = arr;
    printf("%d %d %d\n", *p, *(p + 2), p[4]);
    p = p + 3;
    printf("%d\n", *p);
    p--;
    printf("%d\n", *p);
    int *q = &arr[4];
    printf("diff=%d\n", q - arr);
    printf("%d\n", q > p);

    incr_all(arr, 5);
    int i;
    for (i = 0; i < 5; i++) printf("%d ", *(arr + i));
    printf("\n");

    char msg[] = "pointer";
    printf("%d\n", strlen2(msg));
    char *cp = msg;
    while (*cp) {
        printf("%c.", *cp);
        cp++;
    }
    printf("\n");
    char *str = "literal";
    printf("%s %d\n", str, strlen2(str));

    int *np = 0;
    printf("%d\n", np == 0);
    int *ptrs[3];
    for (i = 0; i < 3; i++) ptrs[i] = &arr[i];
    printf("%d %d %d\n", *ptrs[0], *ptrs[1], *ptrs[2]);
    return 0;
}
