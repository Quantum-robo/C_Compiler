// Feature: printf and scanf (reads the numbers, a character, a word and a hex value from 10_printf_scanf.in)
int main() {
    int a, b;
    char c;
    char word[32];
    int h;
    printf("Enter two numbers: ");
    int n = scanf("%d %d", &a, &b);
    printf("read %d values: %d and %d, sum=%d\n", n, a, b, a + b);
    scanf(" %c", &c);
    printf("char: %c (code %d)\n", c, c);
    scanf("%s", word);
    printf("word: %s (%d letters)\n", word, strlen(word));
    scanf("%x", &h);
    printf("hex: %d\n", h);
    int total = 0, v, count = 0;
    while (scanf("%d", &v) == 1) {
        total += v;
        count++;
    }
    printf("remaining: %d numbers, total=%d\n", count, total);
    return 0;
}
