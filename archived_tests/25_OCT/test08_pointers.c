// Feature: Pointers (address-of, dereference, write through pointer, pointer to pointer, arithmetic)
int main() {
    int x = 5;
    int y = 0;
    int *p = &x;
    int **pp = &p;
    y = *p;
    *p = 10;
    **pp = 20;
    int arr[3];
    int *q = arr;
    q = q + 1;
    *q = 7;
    int z = *p + *q;
    return z + y;
}
