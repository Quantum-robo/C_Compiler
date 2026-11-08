// Feature: Recursive function call (factorial, fibonacci, gcd, power)
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int fib(int n) {
    if (n < 2) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

int power(int base, int exp) {
    if (exp == 0) {
        return 1;
    }
    return base * power(base, exp - 1);
}

int main() {
    int f = factorial(5);
    int fb = fib(7);
    int g = gcd(48, 18);
    int p = power(2, 10);
    return f + fb + g + p;
}
