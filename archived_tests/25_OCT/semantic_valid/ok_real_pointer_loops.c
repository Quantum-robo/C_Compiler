struct P {
    int v;
};
void fill(int *arr, int n) {
    for (int i = 0; i < n; i++) *(arr + i) = i * i;
}
void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}
int main() {
    int arr[5];
    struct P ps[3];
    int n = sizeof(arr) / sizeof(arr[0]);
    int **pp;
    int x = 1;
    int *px = &x;
    fill(arr, n);
    swap(&arr[0], &arr[4]);
    pp = &px;
    **pp = 9;
    for (struct P *p = ps; p < ps + 3; p++) p->v = x;
    char *names[2] = {"ab", "cd"};
    char **q = names;
    return arr[0] + ps[1].v + *q[0];
}
