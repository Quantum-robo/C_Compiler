// EXPECT: destructor name '~B' does not match the class 'A'
class B { public: int v; };
class A { public: int w; ~B() { } };
int main() { return 0; }
