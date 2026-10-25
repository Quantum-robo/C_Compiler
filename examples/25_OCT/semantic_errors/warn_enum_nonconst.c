// EXPECT: is not a simple integer constant
enum E { A = 1 << 3, B, C = 2 };
int main() { int x = A + B + C; return x; }
