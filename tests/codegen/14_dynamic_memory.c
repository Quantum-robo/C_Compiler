// Feature: dynamic memory allocation (new, new[], delete, malloc, free; linked list on the heap)
struct Node {
    int value;
    struct Node *next;
};

struct Node *push(struct Node *head, int v) {
    struct Node *n = new struct Node;
    n->value = v;
    n->next = head;
    return n;
}

int main() {
    int *single = new int;
    *single = 42;
    printf("%d\n", *single);

    int n = 6;
    int *arr = new int[n];
    int i;
    for (i = 0; i < n; i++) arr[i] = i * i;
    int total = 0;
    for (i = 0; i < n; i++) total += arr[i];
    printf("total=%d\n", total);

    int *buf = malloc(4 * sizeof(int));
    buf[0] = 5; buf[1] = 6; buf[2] = 7; buf[3] = 8;
    printf("%d\n", buf[0] + buf[1] + buf[2] + buf[3]);

    char *text = new char[6];
    text[0] = 'h'; text[1] = 'i'; text[2] = 0;
    printf("%s\n", text);

    struct Node *head = 0;
    for (i = 1; i <= 5; i++) head = push(head, i * 10);
    struct Node *p = head;
    int count = 0;
    while (p != 0) {
        printf("%d ", p->value);
        p = p->next;
        count++;
    }
    printf("\ncount=%d\n", count);

    while (head != 0) {
        struct Node *next = head->next;
        delete head;
        head = next;
    }
    delete single;
    delete[] arr;
    free(buf);
    delete[] text;

    int **grid = new int*[3];
    for (i = 0; i < 3; i++) {
        grid[i] = new int[3];
        int j;
        for (j = 0; j < 3; j++) grid[i][j] = i * 3 + j;
    }
    printf("%d %d %d\n", grid[0][0], grid[1][1], grid[2][2]);
    return 0;
}
