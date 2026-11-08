// #define constants, casts, a type name used as a variable name, class B; forward declarations
#define SIZE 6
#define LIMIT (SIZE * 2)
#define GREETING "hello"
#define FLAG_A 1
#define FLAG_B 2   // a trailing comment is not part of the macro
#define EMPTY
#define ALIAS SIZE

class B;

class A {
public:
    B *peer;
    int v;
};

class B {
public:
    int w;
};

struct node {
    int val;
    struct node *next;
};

int total(struct node *node) {
    int s = 0;
    while (node != 0) {
        s += node->val;
        node = node->next;
    }
    return s;
}

int main() {
    int arr[SIZE];
    int i;
    for (i = 0; i < SIZE; i++) arr[i] = i * LIMIT;
    printf("%s %d %d %d\n", GREETING, arr[SIZE - 1], LIMIT, ALIAS + FLAG_A + FLAG_B);
    printf("%d %d\n", sizeof(arr) / sizeof(arr[0]), EMPTY 7);

    int *p = (int *)malloc(4 * sizeof(int));
    for (i = 0; i < 4; i++) p[i] = i + 1;
    printf("%d\n", p[0] + p[3]);
    char *bytes = (char *)p;
    printf("%d %d\n", bytes[0], bytes[4]);
    void *v = (void *)p;
    int *q = (int *)v;
    printf("%d\n", q[2]);

    int big = 321;
    printf("%d %d %d\n", (char)big, (char)200, (short)70000);
    printf("%d %d %d\n", (unsigned char)200, (unsigned char)big, (unsigned short)70000);
    printf("%d %d %d\n", (int)'a', (char)(big + 1), (bool)5 + (bool)0);
    printf("%d\n", (int)(char)-1);
    char c = 'x';
    printf("%d %c\n", (int)c + 1, (char)(c - 1));
    printf("%d %d\n", sizeof(short), sizeof(unsigned int));
    free(p);

    A a;
    B b;
    b.w = 9;
    a.peer = &b;
    a.v = 4;
    printf("%d %d\n", a.peer->w, a.v);

    struct node x, y;
    x.val = 10; x.next = &y;
    y.val = 20; y.next = 0;
    struct node *node = &x;
    struct node node2 = y;
    printf("%d %d %d\n", total(node), node->val, node2.val);
    return 0;
}
