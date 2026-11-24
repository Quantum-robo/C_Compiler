// EXPECT: field 'inner' has incomplete type 'Node'
struct Node {
    int value;
    struct Node inner;
};

int main() {
    return 0;
}
