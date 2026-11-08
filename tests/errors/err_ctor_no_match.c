// EXPECT: no matching constructor for 'A' with 0 argument(s)
class A { public: int v; A(int x) { v = x; } };
int main() { A a; return 0; }
