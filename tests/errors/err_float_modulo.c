// EXPECT: invalid operands to binary '%' (have 'double' and 'int')
int main() {
    double d = 7.5;
    int r = d % 2;
    return r;
}
