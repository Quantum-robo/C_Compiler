// Integration: large structs (copied through a loop), a big local array, global initialisers, nested struct calls
struct Big {
    int id;
    int cells[40];
    char label[8];
};

struct Pair {
    struct Big first;
    struct Big second;
};

struct Big g = {7, {1, 2, 3}, "gl"};
int primes[5] = {2, 3, 5, 7, 11};
char greeting[16] = "hello";
const int LIMIT = 6;
int sized[LIMIT];

struct Big make(int id, int fill) {
    struct Big b;
    int i;
    b.id = id;
    for (i = 0; i < 40; i++) b.cells[i] = fill + i;
    b.label[0] = 'A' + id;
    b.label[1] = 0;
    return b;
}

int total(struct Big b) {
    int i, t = b.id;
    for (i = 0; i < 40; i++) t += b.cells[i];
    return t;
}

int main() {
    printf("%d %d %d %d %s\n", g.id, g.cells[0], g.cells[2], g.cells[3], g.label);
    printf("%d %d %s %d\n", primes[4], primes[0] + primes[3], greeting, sizeof(sized));

    struct Big a = make(1, 100);
    struct Big b = a;
    b.cells[5] = 0;
    printf("%d %d %s\n", total(a), total(b), b.label);

    struct Pair p;
    p.first = a;
    p.second = make(2, 0);
    printf("%d %d\n", p.first.cells[39], p.second.cells[39]);
    p.second = p.first;
    printf("%d %s\n", p.second.id, p.second.label);
    printf("%d\n", total(p.second) - total(p.first));

    int big[10000];
    int i;
    for (i = 0; i < 10000; i++) big[i] = i % 7;
    int s = 0;
    for (i = 0; i < 10000; i++) s += big[i];
    printf("%d %d\n", s, sizeof(big));
    printf("%d\n", sizeof(struct Pair));
    return 0;
}
