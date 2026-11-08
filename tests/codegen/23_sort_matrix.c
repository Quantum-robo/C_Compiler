// Integration: sorting and matrix work on global arrays, 2D arrays passed to functions, char counting
int data[10] = {64, 25, 12, 22, 11, 90, 5, 77, 31, 8};
int grid[3][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
char text[48] = "the quick brown fox jumps over the lazy dog";

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

void bubble_sort(int a[], int n) {
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - i - 1; j++)
            if (a[j] > a[j + 1]) swap(&a[j], &a[j + 1]);
}

void insertion_sort(int a[], int n) {
    int i;
    for (i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

int binary_search(int a[], int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (a[mid] == key) return mid;
        if (a[mid] < key) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

void print_array(int a[], int n) {
    int i;
    for (i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");
}

int row_sum(int m[][4], int r) {
    int s = 0, c;
    for (c = 0; c < 4; c++) s += m[r][c];
    return s;
}

void transpose(int src[][4], int dst[][3]) {
    int r, c;
    for (r = 0; r < 3; r++)
        for (c = 0; c < 4; c++) dst[c][r] = src[r][c];
}

int main() {
    print_array(data, 10);
    bubble_sort(data, 10);
    print_array(data, 10);
    printf("index of 25: %d, of 26: %d\n", binary_search(data, 10, 25), binary_search(data, 10, 26));

    int other[8] = {9, 4, 7, 1, 8, 2, 6, 3};
    insertion_sort(other, 8);
    print_array(other, 8);

    int r;
    for (r = 0; r < 3; r++) printf("row %d sum %d\n", r, row_sum(grid, r));
    int t[4][3];
    transpose(grid, t);
    for (r = 0; r < 4; r++) printf("%d %d %d\n", t[r][0], t[r][1], t[r][2]);

    int counts[26];
    int i;
    for (i = 0; i < 26; i++) counts[i] = 0;
    for (i = 0; text[i] != 0; i++) {
        if (text[i] >= 'a' && text[i] <= 'z') counts[text[i] - 'a']++;
    }
    int best = 0;
    for (i = 1; i < 26; i++) if (counts[i] > counts[best]) best = i;
    printf("most common: %c x%d, length %d\n", 'a' + best, counts[best], strlen(text));
    return 0;
}
