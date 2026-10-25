// Edge: compound assignment and ++/-- through pointers and array elements
int main() {
    int x = 1;
    int *p = &x;
    int arr[3];
    *p += 5;
    (*p)++;
    ++*p;
    (*p)--;
    arr[1] = 4;
    arr[1] *= 2;
    arr[2]++;
    --arr[0];
    int y = *p++;
    return x + y;
}
