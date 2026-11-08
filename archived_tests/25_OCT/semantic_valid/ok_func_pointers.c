int add(int a, int b) { return a + b; }
int apply(int (*f)(int, int), int x) { return f(x, x); }
int main() {
    int (*fp)(int, int) = add;
    int r = fp(1, 2);
    return r + apply(add, 3);
}
