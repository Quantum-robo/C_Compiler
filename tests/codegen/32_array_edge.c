// Array edge cases: arrays of strings, arrays in structs, sizeof, negative index through a pointer, char grids
struct Table {
    int n;
    int cells[4];
    char tag[6];
};

int zeros[5];
int primes[] = {2, 3, 5, 7, 11, 13};
char *names[] = {"ann", "bob", "carol"};
char board[3][3];

int sum_table(struct Table t) {
    int i, s = 0;
    for (i = 0; i < t.n; i++) s += t.cells[i];
    t.cells[0] = 999;
    return s;
}

void reverse(char s[]) {
    int i = 0, j = strlen(s) - 1;
    while (i < j) {
        char t = s[i];
        s[i] = s[j];
        s[j] = t;
        i++;
        j--;
    }
}

int is_palindrome(char *s) {
    int i = 0, j = strlen(s) - 1;
    while (i < j) {
        if (s[i] != s[j]) return 0;
        i++;
        j--;
    }
    return 1;
}

void clear(int a[], int n) {
    int i;
    for (i = 0; i < n; i++) a[i] = 0;
}

int main() {
    int i, j;
    printf("%d %d %d\n", zeros[0], zeros[4], sizeof(zeros) / sizeof(zeros[0]));
    printf("%d %d\n", sizeof(primes) / sizeof(int), primes[5]);
    for (i = 0; i < 3; i++) printf("%s(%d) ", names[i], strlen(names[i]));
    printf("\n");

    struct Table t;
    t.n = 3;
    for (i = 0; i < 4; i++) t.cells[i] = (i + 1) * 10;
    strcpy(t.tag, "tbl");
    struct Table u = t;
    printf("%d %d\n", sum_table(t), t.cells[0]);
    u.cells[1] = 0;
    printf("%d %d %s %d\n", t.cells[1], u.cells[1], u.tag, sizeof(struct Table));

    int arr[6] = {5, 4, 3, 2, 1, 0};
    int *p = arr + 3;
    printf("%d %d %d\n", p[-1], p[-3], *(p - 2));
    clear(arr + 2, 3);
    for (i = 0; i < 6; i++) printf("%d ", arr[i]);
    printf("\n");

    char word[] = "level";
    char other[] = "stressed";
    printf("%d %d\n", is_palindrome(word), is_palindrome(other));
    reverse(other);
    printf("%s\n", other);

    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++) board[i][j] = (i + j) % 2 ? 'X' : 'O';
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) printf("%c", board[i][j]);
        printf("\n");
    }
    printf("%d\n", sizeof(board));

    int a2[3][2] = {{1, 2}, {3, 4}, {5, 6}};
    int *flat = &a2[0][0];
    int s = 0;
    for (i = 0; i < 6; i++) s += flat[i] * (i + 1);
    printf("%d\n", s);

    int counts[10];
    for (i = 0; i < 10; i++) counts[i] = 0;
    char digits[] = "31415926535897932384";
    for (i = 0; digits[i] != 0; i++) counts[digits[i] - '0']++;
    for (i = 0; i < 10; i++) printf("%d", counts[i]);
    printf("\n");
    return 0;
}
