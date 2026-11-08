// Feature: do-while loops (body runs at least once)
int main() {
    int i = 0;
    do {
        printf("i=%d\n", i);
        i++;
    } while (i < 3);

    int j = 10;
    do {
        printf("once j=%d\n", j);
    } while (j < 5);

    int n = 6, f = 1;
    do {
        f *= n;
        n--;
    } while (n > 1);
    printf("6!=%d\n", f);

    int x = 0;
    do {
        x++;
        if (x == 2) continue;
        if (x == 5) break;
        printf("x=%d\n", x);
    } while (x < 10);
    return 0;
}
