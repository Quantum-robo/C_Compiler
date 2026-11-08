// EXPECT: did you mean to use '->'
struct P { int x; };
int main() {
    struct P q;
    struct P *pp = &q;
    pp.x = 1;
    return 0;
}
