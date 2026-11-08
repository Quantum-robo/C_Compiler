// Recursion edge cases: deep recursion, backtracking, trees, recursion on arrays and strings, memoization
struct Tree {
    int key;
    struct Tree *left;
    struct Tree *right;
};

struct Tree *insert(struct Tree *t, int key) {
    if (t == 0) {
        struct Tree *n = new struct Tree;
        n->key = key;
        n->left = 0;
        n->right = 0;
        return n;
    }
    if (key < t->key) t->left = insert(t->left, key);
    else t->right = insert(t->right, key);
    return t;
}

void inorder(struct Tree *t) {
    if (t == 0) return;
    inorder(t->left);
    printf("%d ", t->key);
    inorder(t->right);
}

int height(struct Tree *t) {
    int l, r;
    if (t == 0) return 0;
    l = height(t->left);
    r = height(t->right);
    return 1 + (l > r ? l : r);
}

int count_nodes(struct Tree *t) { return t == 0 ? 0 : 1 + count_nodes(t->left) + count_nodes(t->right); }

int depth_sum(int n) { return n == 0 ? 0 : n + depth_sum(n - 1); }

int queens_cols[8];
int solutions = 0;

int safe(int row, int col) {
    int r;
    for (r = 0; r < row; r++) {
        int c = queens_cols[r];
        if (c == col || c - col == r - row || c - col == row - r) return 0;
    }
    return 1;
}

void place(int row, int n) {
    int col;
    if (row == n) {
        solutions++;
        return;
    }
    for (col = 0; col < n; col++) {
        if (safe(row, col)) {
            queens_cols[row] = col;
            place(row + 1, n);
        }
    }
}

void permute(char *s, int k, int n) {
    int i;
    if (k == n) {
        printf("%s ", s);
        return;
    }
    for (i = k; i < n; i++) {
        char t = s[k]; s[k] = s[i]; s[i] = t;
        permute(s, k + 1, n);
        t = s[k]; s[k] = s[i]; s[i] = t;
    }
}

int memo[40];
int fib(int n) {
    if (n < 2) return n;
    if (memo[n] != 0) return memo[n];
    memo[n] = fib(n - 1) + fib(n - 2);
    return memo[n];
}

void quicksort(int a[], int lo, int hi) {
    int i, j, pivot, t;
    if (lo >= hi) return;
    pivot = a[(lo + hi) / 2];
    i = lo;
    j = hi;
    while (i <= j) {
        while (a[i] < pivot) i++;
        while (a[j] > pivot) j--;
        if (i <= j) {
            t = a[i]; a[i] = a[j]; a[j] = t;
            i++;
            j--;
        }
    }
    quicksort(a, lo, j);
    quicksort(a, i, hi);
}

int str_len(char *s) { return *s == 0 ? 0 : 1 + str_len(s + 1); }

void print_rev(char *s) {
    if (*s == 0) return;
    print_rev(s + 1);
    printf("%c", *s);
}

int binary(int n) { return n < 2 ? n : n % 2 + 10 * binary(n / 2); }

int main() {
    struct Tree *root = 0;
    int keys[9] = {50, 30, 70, 20, 40, 60, 80, 35, 45};
    int i;
    for (i = 0; i < 9; i++) root = insert(root, keys[i]);
    inorder(root);
    printf("\nheight=%d nodes=%d\n", height(root), count_nodes(root));

    printf("%d\n", depth_sum(5000));
    place(0, 6);
    printf("queens(6)=%d\n", solutions);
    char word[] = "abc";
    permute(word, 0, 3);
    printf("\n%d %d\n", fib(30), fib(39));
    int data[10] = {9, 3, 7, 1, 8, 2, 6, 4, 10, 5};
    quicksort(data, 0, 9);
    for (i = 0; i < 10; i++) printf("%d ", data[i]);
    printf("\n%d ", str_len("recursion"));
    print_rev("stressed");
    printf(" %d %d\n", binary(10), binary(255));
    return 0;
}
