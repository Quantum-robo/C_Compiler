// Constructors: initializer lists, member objects (construction in declaration order, destruction in reverse),
// implicit constructors, constructors that call methods, overloading by the number of arguments
class Part {
public:
    int v;
    Part() {
        v = -1;
        printf("Part()\n");
    }
    Part(int x) : v(x) { printf("Part(%d)\n", v); }
    ~Part() { printf("~Part(%d)\n", v); }
};

class Whole {
public:
    Part a;
    Part b;
    int n;
    Whole() : b(7), n(3) { printf("Whole() n=%d\n", n); }
    Whole(int k) : a(k), b(k + 1) {
        n = k;
        printf("Whole(%d)\n", k);
    }
    ~Whole() { printf("~Whole(%d)\n", n); }
};

class Holder {
public:
    Part p[2];
    int tag;
};

class Outer {
public:
    Holder h;
    Part extra;
};

class Account {
public:
    int balance;
    int ops;
    Account(int start) : balance(start), ops(0) { record(); }
    Account(int start, int bonus) {
        balance = start + bonus;
        ops = 0;
        record();
        record();
    }
    void record() { ops++; }
    int total() { return balance * 10 + ops; }
};

struct Pair {
    int x;
    int y;
    Pair(int a, int b) : x(a), y(b) {}
    int sum() { return x + y; }
};

int main() {
    printf("-- w1\n");
    Whole w1;
    printf("-- w2\n");
    Whole w2(10);
    printf("-- holder\n");
    Holder h;
    h.tag = 4;
    printf("-- outer\n");
    {
        Outer o;
        o.extra.v = 99;
        printf("outer built\n");
    }
    printf("-- heap\n");
    Whole *hw = new Whole(20);
    delete hw;
    Account acc(100);
    Account bonus(100, 5);
    printf("%d %d\n", acc.total(), bonus.total());
    Pair pr(3, 4);
    printf("%d\n", pr.sum());
    int plain(7);
    printf("%d\n", plain);
    printf("-- end\n");
    return 0;
}
