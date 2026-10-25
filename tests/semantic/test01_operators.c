int main() {
    int a = 10;
    int b = 3;
    int add = a + b;
    int sub = a - b;
    int mul = a * b;
    int div = a / b;
    int mod = a % b;

    bool land = (a > 5) && (b < 5);
    bool lor  = (a == 10) || (b == 0);
    bool lnot = !land;

    int band = a & b;
    int bor  = a | b;
    int bxor = a ^ b;
    int shl  = a << 1;
    int shr  = a >> 1;

    a += 5;
    b *= 2;
    return add + sub;
}
