// EXPECT: duplicate case value '0'
enum E { A, B = 0 };
int main() { int x = 0; switch (x) { case A: break; case B: break; } return 0; }
