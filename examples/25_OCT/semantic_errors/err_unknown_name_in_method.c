// EXPECT: 'zzz' was not declared in this scope
class C {
public:
    int a;
    int get() { return zzz; }
};
int main() { return 0; }
