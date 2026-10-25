// EXPECT: 'hidden' is private within this context
class K {
private:
    void hidden() { return; }
public:
    int x;
};
int main() {
    class K k;
    k.hidden();
    return 0;
}
