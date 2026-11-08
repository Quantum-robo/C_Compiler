// Feature: all arithmetic and logical operators
int calls = 0;

int bump(int v) {
    calls++;
    return v;
}

int main() {
    int a = 17;
    int b = 5;
    int c = -3;
    printf("%d %d %d %d %d\n", a + b, a - b, a * b, a / b, a % b);
    printf("%d %d %d\n", c * b, c / 2, c % 2);
    printf("%d %d %d\n", -a, +b, ~a);
    printf("%d %d %d\n", a & b, a | b, a ^ b);
    printf("%d %d %d\n", a << 2, a >> 1, c >> 1);
    printf("%d %d %d %d %d %d\n", a == b, a != b, a < b, a <= b, a > b, a >= b);
    printf("%d %d %d\n", a && b, a || 0, !a);
    printf("%d %d\n", (a > b) && (b > c), (a < b) || (c < 0));

    int x = 10;
    x += 5; printf("%d\n", x);
    x -= 3; printf("%d\n", x);
    x *= 2; printf("%d\n", x);
    x /= 4; printf("%d\n", x);
    x %= 4; printf("%d\n", x);
    x <<= 3; printf("%d\n", x);
    x >>= 1; printf("%d\n", x);
    x |= 1; printf("%d\n", x);
    x &= 6; printf("%d\n", x);
    x ^= 5; printf("%d\n", x);

    int y = x++;
    printf("%d %d\n", y, x);
    y = ++x;
    printf("%d %d\n", y, x);
    y = x--;
    printf("%d %d\n", y, x);
    y = --x;
    printf("%d %d\n", y, x);

    printf("%d\n", 2 + 3 * 4 - 6 / 2 % 2);
    printf("%d\n", (2 + 3) * (4 - 1));
    int m = a > b ? a : b;
    printf("%d %d\n", m, a < b ? a : b);

    if (bump(0) && bump(1)) { printf("never\n"); }
    printf("calls=%d\n", calls);
    if (bump(1) || bump(1)) { printf("taken\n"); }
    printf("calls=%d\n", calls);
    return 0;
}
