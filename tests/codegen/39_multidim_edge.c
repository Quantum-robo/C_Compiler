// Multi-dimensional array edge cases: 3D arrays as parameters, global 2D data, arrays inside structs,
// Pascal's triangle, in-place transpose, tic-tac-toe board, sizeof forms
struct Grid {
    int cells[3][3];
    char name[4];
};

int cube[2][3][4];
int weights[3][2] = {{1, 2}, {3, 4}, {5, 6}};
char words[4][6] = {"one", "two", "three", "four"};

int sum3(int a[][3][4], int n) {
    int i, j, k, s = 0;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3; j++)
            for (k = 0; k < 4; k++) s += a[i][j][k];
    return s;
}

void transpose(int m[3][3]) {
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = i + 1; j < 3; j++) {
            int t = m[i][j];
            m[i][j] = m[j][i];
            m[j][i] = t;
        }
}

int diag(struct Grid g) {
    int i, s = 0;
    for (i = 0; i < 3; i++) s += g.cells[i][i];
    g.cells[0][0] = -1;
    return s;
}

int winner(char b[3][3]) {
    int i;
    for (i = 0; i < 3; i++) {
        if (b[i][0] != '.' && b[i][0] == b[i][1] && b[i][1] == b[i][2]) return b[i][0];
        if (b[0][i] != '.' && b[0][i] == b[1][i] && b[1][i] == b[2][i]) return b[0][i];
    }
    if (b[1][1] != '.' && b[0][0] == b[1][1] && b[1][1] == b[2][2]) return b[1][1];
    return '-';
}

int main() {
    int i, j, k;
    for (i = 0; i < 2; i++)
        for (j = 0; j < 3; j++)
            for (k = 0; k < 4; k++) cube[i][j][k] = i * 100 + j * 10 + k;
    printf("%d %d %d\n", cube[1][2][3], cube[0][1][2], sum3(cube, 2));
    printf("%d %d %d\n", sizeof(cube), sizeof(cube[0]), sizeof(cube[0][0]));

    int sw = 0;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 2; j++) sw += weights[i][j] * (i + 1);
    printf("%d %d\n", sw, sizeof(weights) / sizeof(weights[0]));
    for (i = 0; i < 4; i++) printf("%s/%d ", words[i], strlen(words[i]));
    printf("\n");

    struct Grid g;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++) g.cells[i][j] = i * 3 + j + 1;
    strcpy(g.name, "grd");
    printf("%d %d\n", diag(g), g.cells[0][0]);
    transpose(g.cells);
    printf("%d %d %d %s\n", g.cells[0][1], g.cells[1][0], g.cells[2][0], g.name);

    int tri[6][6];
    for (i = 0; i < 6; i++)
        for (j = 0; j < 6; j++) tri[i][j] = 0;
    for (i = 0; i < 6; i++) {
        tri[i][0] = 1;
        for (j = 1; j <= i; j++) tri[i][j] = tri[i - 1][j - 1] + tri[i - 1][j];
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j <= i; j++) printf("%d ", tri[i][j]);
        printf("\n");
    }

    char board[3][3];
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++) board[i][j] = '.';
    printf("%c", winner(board));
    board[0][2] = 'X'; board[1][1] = 'X'; board[2][0] = 'X';
    printf("%c", winner(board));
    board[0][2] = 'O'; board[1][2] = 'O'; board[2][2] = 'O';
    printf("%c\n", winner(board));

    int *row = weights[1];
    printf("%d %d %d\n", row[0], row[1], *(weights[2] + 1));
    return 0;
}
