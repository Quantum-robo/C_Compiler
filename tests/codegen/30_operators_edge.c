// Operator edge cases: narrow types wrapping, chained assignment, comma, nested ?:, signed division, shifts
int g = 0;

int side(int v) {
    g = g * 10 + v;
    return v;
}

int main() {
    char c = 127;
    c++;
    printf("%d\n", c);
    char d = -1;
    printf("%d\n", d & 255);
    short s = 32767;
    s++;
    printf("%d\n", s);
    char e = 200;
    printf("%d\n", e);

    int a = 5, b = 3, k;
    a = b = 9;
    printf("%d %d\n", a, b);
    k = (a++, b++, a + b);
    printf("%d %d %d\n", k, a, b);
    printf("%d %d\n", 1 ? 2 ? 3 : 4 : 5, 0 ? 1 : 0 ? 2 : 3);

    int arr[4] = {1, 2, 3, 4};
    int i = 0;
    arr[i] += 5;
    arr[++i] *= 2;
    arr[i + 1] -= arr[i];
    arr[3] <<= 2;
    printf("%d %d %d %d\n", arr[0], arr[1], arr[2], arr[3]);

    printf("%d %d %d %d\n", -7 / 2, -7 % 2, 7 / -2, 7 % -2);
    printf("%d %d %d\n", -8 >> 1, 1 << 30, 5 >> 5);
    printf("%d %d %d\n", !5 + !0 * 3, ~0, ~5);
    printf("%d %d\n", 5 > 3 > 1, 1 < 2 < 3);
    printf("%d %d\n", 2 + 3 * 4 % 5 - 1, (2 + 3) * 4 % 5 - 1);
    printf("%d %d %d\n", 6 & 3 | 8, 6 ^ 3 & 5, 1 | 2 ^ 3 & 4);
    printf("%d %d\n", 1 + 2 << 3, 100 / 10 / 5);

    if (side(1) && side(0) && side(2)) printf("no\n");
    printf("%d\n", g);
    g = 0;
    if (side(0) || side(3) || side(4)) printf("yes\n");
    printf("%d\n", g);

    int t = 3;
    t <<= 2;
    t |= 1;
    t ^= 3;
    t &= 14;
    t %= 5;
    t += t * 2;
    printf("%d\n", t);

    int x = 10;
    int y = x++ + ++x;
    printf("%d %d\n", x, y > 0);
    int big = 46341;
    printf("%d\n", big * big / 2 > 0);
    int neg = -5;
    printf("%d %d %d\n", neg % 3, -neg % 3, neg / 3);
    printf("%d\n", (1 + 2) * (3 + 4) - (5 - 6) * (7 - 8));
    return 0;
}
