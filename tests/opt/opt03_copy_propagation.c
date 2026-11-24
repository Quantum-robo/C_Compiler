// Optimizer: copy propagation. b and c are copies of a, so their uses read a directly; the copies themselves are
// then never read and are removed as dead code. A variable whose address is taken (y) is never propagated,
// because a store through a pointer could change it.
int copies(int a) {
    int b = a;
    int c = b;
    int d = c + b;
    return d;
}

int alias(int x) {
    int y = x;
    int *p = &y;
    *p = 7;
    return y + 1;
}

int main() {
    return copies(3) + alias(4);
}
