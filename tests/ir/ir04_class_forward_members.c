// IR: methods may use members declared after them, and the size of their own class
struct P2 { int a; int b; };

class Node {
public:
    int get() { return val; }
    int nextVal() { return next->val; }
    int posb() { return pos.b; }
    class Node *spawn() {
        class Node *n = new class Node;
        n->val = sizeof(class Node);
        return n;
    }
    void link(class Node *other) { next = other; }
private:
    int val;
    class Node *next;
    struct P2 pos;
};

int main() {
    class Node a;
    class Node *b = a.spawn();
    a.link(b);
    return a.get() + a.nextVal() + a.posb();
}
