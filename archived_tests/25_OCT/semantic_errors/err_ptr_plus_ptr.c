// EXPECT: invalid operands to binary '+'
int main() {
    int *p;
    int *q;
    int *r = p + q;
    return 0;
}
