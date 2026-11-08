// Constructors and destructors: order across scopes, return / break / continue, new / delete, arrays, globals
class Tracer {
public:
    int id;
    Tracer() {
        id = 0;
        printf("ctor default\n");
    }
    Tracer(int i) {
        id = i;
        printf("ctor %d\n", id);
    }
    ~Tracer() { printf("dtor %d\n", id); }
};

Tracer global_a(100);
Tracer global_b;

void scope_test() {
    Tracer a(1);
    {
        Tracer b(2);
        Tracer c;
        printf("inner\n");
    }
    printf("after inner\n");
    Tracer pair[2];
    pair[0].id = 7;
    pair[1].id = 8;
    printf("end of function\n");
}

int early(int n) {
    Tracer t(10 + n);
    if (n > 0) return n;
    Tracer u(20);
    return 0;
}

int main() {
    printf("main start\n");
    scope_test();
    printf("early: %d\n", early(1));
    printf("early: %d\n", early(0));

    int i;
    for (i = 0; i < 2; i++) {
        Tracer l(30 + i);
        if (i == 0) continue;
        printf("loop body end\n");
    }
    int k = 0;
    while (k < 3) {
        Tracer w(40 + k);
        k++;
        if (k == 2) break;
    }
    switch (k) {
        case 2: {
            Tracer s(60);
            break;
        }
        default:
            printf("not reached\n");
    }

    Tracer *p = new Tracer(50);
    Tracer *q = new Tracer;
    printf("p=%d q=%d\n", p->id, q->id);
    delete p;
    Tracer *arr = new Tracer[2];
    arr[1].id = 9;
    delete[] arr;
    delete q;
    Tracer *none = 0;
    delete none;
    printf("main end\n");
    return 0;
}
