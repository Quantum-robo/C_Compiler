struct Node {
    int data;
    struct Node *next;
};
struct Node *push(struct Node *head, int v) {
    struct Node *n = malloc(sizeof(struct Node));
    n->data = v;
    n->next = head;
    return n;
}
int sumList(struct Node *h) {
    int s = 0;
    while (h != NULL) {
        s += h->data;
        h = h->next;
    }
    return s;
}
int main() {
    struct Node *head = NULL;
    for (int i = 0; i < 5; i++)
        head = push(head, i);
    printf("%d\n", sumList(head));
    return 0;
}
