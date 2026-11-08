// EXPECT: 'v' is private within this context
class D {
    int v;
};
int main() {
    class D d;
    d.v = 1;
    return 0;
}
