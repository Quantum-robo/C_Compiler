// Feature: while loops
int main() {
    int n = 12345;
    int digits = 0;
    int rev = 0;
    while (n > 0) {
        rev = rev * 10 + n % 10;
        n /= 10;
        digits++;
    }
    printf("digits=%d reversed=%d\n", digits, rev);

    int a = 48, b = 18;
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    printf("gcd=%d\n", a);

    int i = 0;
    while (1) {
        i++;
        if (i % 2 == 0) continue;
        if (i > 9) break;
        printf("odd %d\n", i);
    }

    int count = 0;
    int k = 100;
    while (k > 1) {
        k = k % 2 == 0 ? k / 2 : 3 * k + 1;
        count++;
    }
    printf("collatz steps=%d\n", count);
    return 0;
}
