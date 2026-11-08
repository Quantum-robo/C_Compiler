// Edge: && / || short-circuit (side effects must be skipped), chained logic, ternary evaluates one branch
int side(int v) {
    return v;
}

int main() {
    int a = 0;
    int b = 1;
    int r1 = a && side(1);
    int r2 = b || side(2);
    int r3 = a || b && side(3);
    int r4 = (a < b) ? side(10) : side(20);
    int r5 = a ? 1 : b ? 2 : 3;
    return r1 + r2 + r3 + r4 + r5;
}
