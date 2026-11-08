// EXPECT: duplicate member 'x'
struct S {
    int x;
    int x;
};
int main() { return 0; }
