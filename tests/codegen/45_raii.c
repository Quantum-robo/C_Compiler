// RAII: memory owned by an object (allocated in the constructor, released in the destructor), objects returned
// from functions, objects passed by reference and by value, objects as array elements and in structs
class IntArray {
private:
    int *data;
    int n;

public:
    IntArray(int size) {
        n = size;
        data = new int[size];
        int i;
        for (i = 0; i < size; i++) data[i] = 0;
        printf("alloc %d\n", n);
    }
    ~IntArray() {
        delete[] data;
        printf("free %d\n", n);
    }
    void set(int i, int v) { data[i] = v; }
    int get(int i) { return data[i]; }
    int sum() {
        int i, s = 0;
        for (i = 0; i < n; i++) s += data[i];
        return s;
    }
    int size() { return n; }
};

class Text {
private:
    char *buf;
    int len;

public:
    Text(char *s) {
        len = strlen(s);
        buf = (char *)malloc(len + 1);
        strcpy(buf, s);
        printf("Text(%s)\n", buf);
    }
    ~Text() {
        printf("~Text(%s)\n", buf);
        free(buf);
    }
    int length() { return len; }
    char *c_str() { return buf; }
    void upcase() {
        int i;
        for (i = 0; i < len; i++) buf[i] = toupper(buf[i]);
    }
};

class Tracer {
public:
    int id;
    Tracer(int i) {
        id = i;
        printf("Tracer(%d)\n", id);
    }
    ~Tracer() { printf("~Tracer(%d)\n", id); }
};

Tracer make(int i) {
    Tracer t(i);
    printf("made %d\n", i);
    return t;
}

void fill(IntArray &a, int base) {
    int i;
    for (i = 0; i < a.size(); i++) a.set(i, base + i);
}

void consume(Tracer t) { printf("consume %d\n", t.id); }

int total(IntArray *p) { return p->sum(); }

struct Slot {
    Tracer first;
    int n;
    Slot(int k) : first(k * 10), n(k) { printf("Slot(%d)\n", n); }
    ~Slot() { printf("~Slot(%d)\n", n); }
};

int main() {
    {
        IntArray a(4);
        fill(a, 10);
        printf("%d %d\n", a.sum(), a.get(2));
        IntArray b(2);
        b.set(0, 5);
        b.set(1, 6);
        printf("%d\n", b.sum() + total(&a));
    }
    printf("after block\n");

    Text t("hello");
    t.upcase();
    printf("%s %d\n", t.c_str(), t.length());

    Tracer x = make(5);
    printf("x=%d\n", x.id);
    Tracer y(6);
    consume(y);
    printf("after consume\n");

    IntArray *heap = new IntArray(3);
    heap->set(1, 8);
    printf("%d\n", heap->sum());
    delete heap;

    Slot s1(1);
    Slot s2(2);
    printf("end\n");
    return 0;
}
