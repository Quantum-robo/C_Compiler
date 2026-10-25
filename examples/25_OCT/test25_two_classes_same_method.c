// Edge: two classes with the same method name, method calling a sibling method, call via pointer
class A {
public:
    int v;
    int get() {
        return v;
    }
    int twice() {
        return get() + get();
    }
};

class B {
public:
    int w;
    int get() {
        return w * 10;
    }
};

int main() {
    class A a;
    class B b;
    class A *pa = &a;
    a.v = 3;
    b.w = 4;
    int x = a.get();
    int y = b.get();
    int z = pa->twice();
    return x + y + z;
}
