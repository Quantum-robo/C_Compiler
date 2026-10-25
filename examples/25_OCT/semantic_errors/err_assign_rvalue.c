// EXPECT: lvalue required as left operand of assignment
int main() {
    int x = 1;
    5 = x;
    return x;
}
