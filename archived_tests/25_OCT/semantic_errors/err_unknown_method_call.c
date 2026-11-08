// EXPECT: 'nothere' was not declared in this scope
class C {
public:
    int a;
    int get() { return nothere(); }
};
int main() { return 0; }
