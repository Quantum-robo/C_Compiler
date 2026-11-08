// EXPECT: lvalue required as unary '&' operand
int main() {
    int a = 1;
    int *p = &(a + 1);
    return 0;
}
