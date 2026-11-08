// Edge: array members, pointer-to-struct array access, struct brace initializer
struct Buf {
    int len;
    int data[4];
};

struct Pair {
    int first;
    int second;
};

int main() {
    struct Buf b;
    struct Buf *p = &b;
    struct Pair pr = {7, 9};
    b.len = 4;
    b.data[0] = 1;
    p->data[1] = 2;
    p->data[p->len - 1] = 3;
    int v = p->data[1] + b.data[0];
    return v + pr.second;
}
