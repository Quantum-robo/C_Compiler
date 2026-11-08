// EXPECT: cannot convert 'P' to 'int'
struct P { int x; };
int main() {
    struct P p;
    int x = p;
    return x;
}
