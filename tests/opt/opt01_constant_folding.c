// Optimizer: constant folding. The parser already folds literal arithmetic (2 * 3 + 4); the optimizer also
// folds operations whose operands only become constants after propagation, including shifts, comparisons,
// unary operators and C's truncating division. Division by zero is left for run time.
int fold(int z) {
    int a = 2 * 3 + 4;
    int b;
    int c;
    b = 10;
    c = b * 4 - 1;
    c = c + (b << 2) + (b >> 1) + (b % 3) + (-b / 4);
    c = c + (b > 5) + (b == 10) + !b + ~b + -b;
    c = c + b / z;
    return a + c;
}

int main() {
    return fold(1);
}
