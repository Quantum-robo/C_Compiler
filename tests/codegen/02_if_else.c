// Feature: if / else if / else, nesting, dangling else
int sign(int n) {
    if (n > 0) return 1;
    else if (n < 0) return -1;
    else return 0;
}

int main() {
    int i;
    for (i = -1; i <= 1; i++) {
        printf("sign(%d) = %d\n", i, sign(i));
    }
    int score = 73;
    if (score >= 90) {
        printf("A\n");
    } else if (score >= 80) {
        printf("B\n");
    } else if (score >= 70) {
        printf("C\n");
    } else {
        printf("F\n");
    }
    int a = 1, b = 0;
    if (a)
        if (b) printf("both\n");
        else printf("only a\n");
    if (!a) printf("not shown\n");
    if (a && !b) { printf("a and not b\n"); }
    int x = 5;
    if (x > 3) {
        if (x > 4) {
            if (x > 5) printf("x > 5\n");
            else printf("x == 5\n");
        }
    }
    return 0;
}
