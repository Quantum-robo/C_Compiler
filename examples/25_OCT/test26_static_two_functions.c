// Edge: same static local name in two functions, static array, global initialisers
int g = 10;
int table[3] = {1, 2, 3};

int f() {
    static int n = 100;
    n = n + 1;
    return n;
}

int h() {
    static int n = 200;
    n = n + 2;
    return n;
}

int main() {
    int a = f();
    int b = h();
    return a + b + g + table[2];
}
