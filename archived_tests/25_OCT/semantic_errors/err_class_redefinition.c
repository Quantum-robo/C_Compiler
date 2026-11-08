// EXPECT: redefinition of 'A'
class A { public: int x; };
class A { public: int y; };
int main() { return 0; }
