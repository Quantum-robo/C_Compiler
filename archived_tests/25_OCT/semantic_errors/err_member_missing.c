// EXPECT: 'P' has no member named 'z'
struct P { int x; };
int main() {
    struct P p;
    p.z = 1;
    return 0;
}
