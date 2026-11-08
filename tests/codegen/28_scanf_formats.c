// scanf: literals in the format, widths, %*d suppression, %x %o, signs, failed matches and end of input
// (reads 28_scanf_formats.in)
int main() {
    int a, b, c;
    char s[8];
    int r1 = scanf("%d,%d", &a, &b);
    printf("%d: %d %d\n", r1, a, b);
    int r2 = scanf("%3d%d", &a, &b);
    printf("%d: %d %d\n", r2, a, b);
    int r3 = scanf("%*d %d", &c);
    printf("%d: %d\n", r3, c);
    int r4 = scanf("%5s", s);
    printf("%d: %s\n", r4, s);
    scanf("%s", s);
    printf("rest of the word: %s\n", s);
    int r5 = scanf("%d", &a);
    printf("%d\n", r5);
    scanf("%s", s);
    printf("skipped: %s\n", s);
    int r6 = scanf("%x %o", &a, &b);
    printf("%d: %d %d\n", r6, a, b);
    scanf("%d %d", &a, &b);
    printf("%d %d\n", a, b);
    char x, y;
    scanf("%c%c", &x, &y);
    printf("[%c][%c]\n", x, y);
    int r7 = scanf("%d", &a);
    printf("end: %d\n", r7);
    return 0;
}
