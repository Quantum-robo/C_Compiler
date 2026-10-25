// EXPECT: cannot convert 'int' to 'P'
struct P { int x; };
int main() {
    struct P p;
    p = 5;
    return 0;
}
