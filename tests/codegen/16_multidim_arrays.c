// Feature: multi-dimensional arrays (2D / 3D, initializers, passing rows, matrix multiplication)
int trace(int m[][3], int n) {
    int t = 0;
    int i;
    for (i = 0; i < n; i++) t += m[i][i];
    return t;
}

int main() {
    int m[2][3] = {{1, 2, 3}, {4, 5, 6}};
    int i, j;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) printf("%d ", m[i][j]);
        printf("\n");
    }
    printf("sizeof=%d\n", sizeof(m));

    int cube[2][2][2];
    int n = 0;
    int k;
    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++) cube[i][j][k] = n++;
    printf("%d %d %d\n", cube[0][1][1], cube[1][0][0], cube[1][1][1]);

    int a[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    int b[3][3] = {{1, 0, 0}, {0, 2, 0}, {0, 0, 3}};
    int c[3][3];
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++) {
            c[i][j] = 0;
            for (k = 0; k < 3; k++) c[i][j] += a[i][k] * b[k][j];
        }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) printf("%d ", c[i][j]);
        printf("\n");
    }
    printf("trace=%d\n", trace(c, 3));

    int part[3][4] = {{1}, {2, 2}};
    printf("%d %d %d %d\n", part[0][0], part[0][3], part[1][1], part[2][2]);

    char names[3][8] = {"ab", "cde", "f"};
    for (i = 0; i < 3; i++) printf("%s,", names[i]);
    printf("\n");
    char grid[3][4];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) grid[i][j] = 'a' + i * 3 + j;
        grid[i][3] = 0;
    }
    printf("%s %s %s\n", grid[0], grid[1], grid[2]);
    int *row = a[1];
    printf("%d %d\n", row[0], *(row + 2));
    return 0;
}
