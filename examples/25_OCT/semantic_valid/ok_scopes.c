int g = 1;
int f(int a) {
    int b = a;
    {
        int a = 2;
        b = b + a;
    }
    return b + g;
}
int main() {
    int i = 0;
    for (int j = 0; j < 2; j++) { i += j; }
    for (int j = 0; j < 3; j++) { i += j; }
    { int i2 = 5; i = i + i2; }
    int g = 7;
    return f(i) + g;
}
