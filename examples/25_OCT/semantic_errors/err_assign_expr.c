// EXPECT: lvalue required as left operand of assignment
int main() {
    int a = 1;
    int b = 2;
    (a + b) = 3;
    return a;
}
