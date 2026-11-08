// EXPECT: constructor of 'A' with 1 parameter(s) is already defined
class A { public: int v; A(int x) { v = x; } A(int y) { v = y; } };
int main() { return 0; }
