class Node {
public:
    int val;
    class Node *next;
    int get() { return this->val; }
    void link(class Node *n) { this->next = n; }
    int total() {
        int t = this->val;
        if (this->next != 0) {
            t = t + this->next->get();
        }
        return t;
    }
};
int main() {
    class Node a;
    class Node b;
    a.val = 1;
    b.val = 2;
    a.link(&b);
    return a.total();
}
