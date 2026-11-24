// Optimizer: common subexpression elimination inside a basic block. w * h, h * w (the same value: * is
// commutative) and w + h are each computed once. After w is assigned, w * h must be computed again.
int area(int w, int h) {
    int a = w * h + 1;
    int b = h * w + 2;
    int c = (w + h) * (w + h);
    w = w + 1;
    int d = w * h;
    return a + b + c + d;
}

int main() {
    return area(2, 3);
}
