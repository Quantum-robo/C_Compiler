int add(int a, int b);
int fact(int n);
int isEven(int n);
int isOdd(int n);
int isEven(int n) { if (n == 0) return 1; return isOdd(n - 1); }
int isOdd(int n) { if (n == 0) return 0; return isEven(n - 1); }
int add(int a, int b) { return a + b; }
int fact(int n) { if (n <= 1) return 1; return n * fact(n - 1); }
int zero(void) { return 0; }
int main(void) {
    return add(fact(3), zero()) + isEven(4);
}
