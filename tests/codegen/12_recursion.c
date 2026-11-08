// Feature: recursive function calls
int fact(int n) { return n <= 1 ? 1 : n * fact(n - 1); }
int fib(int n) { if (n < 2) return n; return fib(n - 1) + fib(n - 2); }
int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
int power(int base, int e) {
    if (e == 0) return 1;
    int half = power(base, e / 2);
    return e % 2 == 0 ? half * half : half * half * base;
}
int sum_array(int a[], int n) { return n == 0 ? 0 : a[n - 1] + sum_array(a, n - 1); }
int digits(int n) { return n < 10 ? 1 : 1 + digits(n / 10); }
void print_down(int n) {
    if (n == 0) return;
    printf("%d ", n);
    print_down(n - 1);
}
void hanoi(int n, char from, char to, char via, int *moves) {
    if (n == 0) return;
    hanoi(n - 1, from, via, to, moves);
    (*moves)++;
    if (n == 3) printf("move disk 3 %c->%c\n", from, to);
    hanoi(n - 1, via, to, from, moves);
}
int is_even(int n);
int is_odd(int n) { return n == 0 ? 0 : is_even(n - 1); }
int is_even(int n) { return n == 0 ? 1 : is_odd(n - 1); }
int ackermann(int m, int n) {
    if (m == 0) return n + 1;
    if (n == 0) return ackermann(m - 1, 1);
    return ackermann(m - 1, ackermann(m, n - 1));
}

int main() {
    printf("%d %d %d\n", fact(5), fact(10), fact(1));
    int i;
    for (i = 0; i < 10; i++) printf("%d ", fib(i));
    printf("\n");
    printf("%d %d\n", gcd(48, 18), gcd(17, 5));
    printf("%d %d\n", power(2, 10), power(3, 7));
    int a[6] = {1, 2, 3, 4, 5, 6};
    printf("%d\n", sum_array(a, 6));
    printf("%d %d\n", digits(7), digits(123456));
    print_down(6);
    printf("\n");
    int moves = 0;
    hanoi(5, 'A', 'C', 'B', &moves);
    printf("moves=%d\n", moves);
    printf("%d %d\n", is_even(10), is_odd(7));
    printf("%d\n", ackermann(2, 3));
    return 0;
}
