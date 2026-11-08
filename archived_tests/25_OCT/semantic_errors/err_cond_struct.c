// EXPECT: where a scalar is required
struct P { int x; };
int main() {
    struct P p;
    if (p) {
        return 1;
    }
    return 0;
}
