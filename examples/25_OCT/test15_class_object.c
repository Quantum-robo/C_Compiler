// Feature: Class and object (members, methods, object creation, method calls)
class Counter {
public:
    int value;

    void increment() {
        value = value + 1;
    }

    int get() {
        return value;
    }
};

int main() {
    class Counter c;
    c.value = 0;
    c.increment();
    c.increment();
    int v = c.get();
    class Counter *pc = &c;
    pc->value = v * 10;
    return pc->value;
}
