int sum(int a[], int n) {
    int s = 0;
    for (int i = 0; i < n; i++) { s += a[i]; }
    return s;
}
int sum2(int m[][3], int rows) { return m[rows - 1][2]; }
int main(int argc, char *argv[]) {
    int x = 4;
    int *p = &x;
    int *q = p + 1;
    int diff = q - p;
    int arr[3] = {1, 2, 3};
    int grid[2][3];
    void *v = p;
    int *back = v;
    char c = argv[0][0];
    int *np = 0;
    if (p != 0 && q > p) { x = *p; }
    if (np == 0) { x = 1; }
    return sum(arr, 3) + sum2(grid, 2) + diff + c;
}
