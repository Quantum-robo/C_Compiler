// Feature: Dynamic memory allocation (new / new[] / delete, malloc / free)
int main() {
    int *single = new int;
    *single = 42;
    int n = 5;
    int *arr = new int[n];
    for (int i = 0; i < n; i++) {
        arr[i] = i * 2;
    }
    int *buf = malloc(n * sizeof(n));
    buf[0] = arr[4];
    free(buf);
    delete single;
    delete arr;
    return 0;
}
