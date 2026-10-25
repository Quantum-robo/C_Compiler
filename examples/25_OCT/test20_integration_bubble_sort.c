// Feature: Integration (functions with args, arrays, loops, if, struct, pointers, printf)
struct Stats {
    int min;
    int max;
};

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

int main() {
    int data[5] = {5, 1, 4, 2, 3};
    sort(data, 5);
    struct Stats s;
    s.min = data[0];
    s.max = data[4];
    for (int i = 0; i < 5; i++) {
        printf("%d ", data[i]);
    }
    printf("\nmin=%d max=%d\n", s.min, s.max);
    return 0;
}
