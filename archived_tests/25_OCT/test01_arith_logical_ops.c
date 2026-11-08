// Feature: All arithmetic and logical operators
int main() {
    int a = 20;
    int b = 6;
    int add = a + b;
    int sub = a - b;
    int mul = a * b;
    int div = a / b;
    int mod = a % b;
    int neg = -a;
    int prec = a + b * 2 - (a - b) / 2;

    int lt = a < b;
    int gt = a > b;
    int le = a <= b;
    int ge = a >= b;
    int eq = a == b;
    int ne = a != b;

    int land = (a > 0) && (b > 0);
    int lor = (a < 0) || (b > 0);
    int lnot = !land;

    int band = a & b;
    int bor = a | b;
    int bxor = a ^ b;
    int bnot = ~a;
    int shl = a << 2;
    int shr = a >> 1;

    a += 1;
    a -= 2;
    a *= 3;
    a /= 4;
    a %= 5;
    a++;
    b--;
    ++a;
    --b;
    return add + sub + mul + div + mod;
}
