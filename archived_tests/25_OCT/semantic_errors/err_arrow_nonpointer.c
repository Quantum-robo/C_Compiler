// EXPECT: base operand of '->' is not a pointer
struct P { int x; };
int main() {
    struct P p;
    p->x = 1;
    return 0;
}
