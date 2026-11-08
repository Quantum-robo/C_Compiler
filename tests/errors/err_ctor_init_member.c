// EXPECT: is not a member of
class A { public: int v; A(int x) : nope(x) { v = x; } };
int main() { return 0; }
