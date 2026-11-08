// Dynamic memory edge cases: 2D matrices, arrays of structs and objects, delete of a null pointer, reuse, string copies
struct Cell {
    int row;
    int col;
};

class Stack {
public:
    int *items;
    int size;
    int cap;

    void init(int c) {
        items = new int[c];
        size = 0;
        cap = c;
    }
    void push(int v) {
        if (size == cap) {
            int *bigger = new int[cap * 2];
            int i;
            for (i = 0; i < size; i++) bigger[i] = items[i];
            delete[] items;
            items = bigger;
            cap *= 2;
        }
        items[size++] = v;
    }
    int pop() { return items[--size]; }
    void release() {
        delete[] items;
        items = 0;
    }
};

char *duplicate(char *s) {
    char *copy = new char[strlen(s) + 1];
    strcpy(copy, s);
    return copy;
}

int **make_matrix(int r, int c) {
    int **m = new int*[r];
    int i;
    for (i = 0; i < r; i++) m[i] = new int[c];
    return m;
}

void free_matrix(int **m, int r) {
    int i;
    for (i = 0; i < r; i++) delete[] m[i];
    delete[] m;
}

int main() {
    int **m = make_matrix(3, 4);
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 4; j++) m[i][j] = i * 4 + j;
    int total = 0;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 4; j++) total += m[i][j];
    printf("%d %d\n", total, m[2][3]);
    free_matrix(m, 3);

    struct Cell *cells = new struct Cell[5];
    for (i = 0; i < 5; i++) {
        cells[i].row = i;
        cells[i].col = i * i;
    }
    printf("%d %d\n", cells[4].row, cells[3].col);
    delete[] cells;

    Stack s;
    s.init(2);
    for (i = 1; i <= 10; i++) s.push(i * i);
    int top = s.pop();
    printf("%d %d %d\n", s.size, s.cap, top);
    printf("%d\n", s.pop());
    s.release();

    char *a = duplicate("first");
    char *b = duplicate("second one");
    printf("%s %s %d\n", a, b, strlen(a) + strlen(b));
    a[0] = 'F';
    printf("%s\n", a);
    delete[] a;
    delete[] b;

    int *none = 0;
    delete none;
    delete[] none;

    Stack *stacks = new Stack[3];
    for (i = 0; i < 3; i++) {
        stacks[i].init(4);
        for (j = 0; j <= i; j++) stacks[i].push(j + i * 10);
    }
    int popped = stacks[2].pop();
    printf("%d %d %d\n", stacks[0].size, stacks[2].size, popped);
    for (i = 0; i < 3; i++) stacks[i].release();
    delete[] stacks;

    int checksum = 0;
    for (i = 0; i < 200; i++) {
        int *tmp = new int[i % 17 + 1];
        tmp[0] = i;
        checksum += tmp[0];
        delete[] tmp;
    }
    printf("%d\n", checksum);
    return 0;
}
