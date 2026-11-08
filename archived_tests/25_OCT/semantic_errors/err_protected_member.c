// EXPECT: 'p' is protected within this context
class C {
protected:
    int p;
public:
    int q;
};
int main() {
    class C c;
    c.p = 1;
    return 0;
}
