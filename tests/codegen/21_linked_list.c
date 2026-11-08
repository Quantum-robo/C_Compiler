// Integration: singly linked list on the heap (sorted insert, reverse, search, delete, free)
struct Node {
    int key;
    struct Node *next;
};

struct Node *insert_sorted(struct Node *head, int key) {
    struct Node *n = new struct Node;
    n->key = key;
    if (head == 0 || key < head->key) {
        n->next = head;
        return n;
    }
    struct Node *p = head;
    while (p->next != 0 && p->next->key < key) p = p->next;
    n->next = p->next;
    p->next = n;
    return head;
}

struct Node *reverse(struct Node *head) {
    struct Node *prev = 0;
    while (head != 0) {
        struct Node *next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}

int length(struct Node *p) {
    return p == 0 ? 0 : 1 + length(p->next);
}

struct Node *find(struct Node *p, int key) {
    while (p != 0) {
        if (p->key == key) return p;
        p = p->next;
    }
    return 0;
}

struct Node *remove_key(struct Node *head, int key) {
    if (head == 0) return 0;
    if (head->key == key) {
        struct Node *rest = head->next;
        delete head;
        return rest;
    }
    head->next = remove_key(head->next, key);
    return head;
}

void print_list(struct Node *p) {
    while (p != 0) {
        printf("%d", p->key);
        if (p->next != 0) printf(" -> ");
        p = p->next;
    }
    printf("\n");
}

int main() {
    struct Node *list = 0;
    int values[8] = {5, 3, 9, 1, 7, 3, 8, 2};
    int i;
    for (i = 0; i < 8; i++) list = insert_sorted(list, values[i]);
    print_list(list);
    printf("length=%d\n", length(list));
    list = reverse(list);
    print_list(list);
    struct Node *f = find(list, 7);
    printf("found=%d missing=%d\n", f != 0 ? f->key : -1, find(list, 4) == 0);
    list = remove_key(list, 9);
    list = remove_key(list, 1);
    list = remove_key(list, 42);
    print_list(list);
    printf("length=%d\n", length(list));
    while (list != 0) {
        struct Node *next = list->next;
        delete list;
        list = next;
    }
    printf("empty=%d\n", list == 0);
    return 0;
}
