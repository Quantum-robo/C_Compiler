// Feature: Functions returning references (the call is an lvalue); sizeof does not evaluate its operand
int g = 1;

int &getg() {
    return g;
}

int &pick(int &a, int &b, int c) {
    if (c) {
        return a;
    }
    return b;
}

int main() {
    int x = 1;
    int y = 2;
    int &r = getg();
    r = 10;
    getg() = 20;
    pick(x, y, 1) = 5;
    int v = pick(x, y, 0) + 1;
    int p[3];
    int *q = p;
    int &rr = x;
    int s1 = sizeof *q;
    int s2 = sizeof p[1];
    int s3 = sizeof rr;
    return g + x + y + v + s1 + s2 + s3;
}
