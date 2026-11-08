// EXPECT: lvalue required as left operand of assignment
enum E { A, B };
int main() {
    A = 2;
    return 0;
}
