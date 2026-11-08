// Feature: for loops (counting, nested, missing parts, break / continue inside)
int main() {
    int i, j;
    int sum = 0;
    for (i = 1; i <= 10; i++) sum += i;
    printf("sum=%d\n", sum);

    for (i = 1; i <= 4; i++) {
        for (j = 1; j <= i; j++) printf("*");
        printf("\n");
    }

    for (i = 10; i > 0; i -= 3) printf("%d ", i);
    printf("\n");

    i = 0;
    for (;;) {
        i++;
        if (i == 3) continue;
        if (i > 6) break;
        printf("i=%d\n", i);
    }

    for (int k = 0, m = 10; k < m; k += 3, m -= 1) printf("k=%d m=%d\n", k, m);

    int table[3][3];
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            table[i][j] = (i + 1) * (j + 1);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) printf("%d ", table[i][j]);
        printf("\n");
    }
    return 0;
}
