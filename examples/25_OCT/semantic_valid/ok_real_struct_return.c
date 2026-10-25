struct V {
    int x;
    int y;
};
struct V make(int a, int b) {
    struct V v;
    v.x = a;
    v.y = b;
    return v;
}
int dot(struct V a, struct V b) {
    return a.x * b.x + a.y * b.y;
}
int main() {
    struct V p = make(1, 2);
    struct V q = make(3, 4);
    return dot(p, q);
}
