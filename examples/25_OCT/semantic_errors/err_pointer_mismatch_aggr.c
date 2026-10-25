// EXPECT: incompatible pointer types
struct A { int x; };
struct B { int y; };
int main() {
    struct A *pa;
    struct B *pb;
    pa = pb;
    return 0;
}
