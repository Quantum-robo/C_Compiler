// Feature: Reference parameters (caller passes the address), prototypes, forwarding
void late(int &x);

void inc(int &x) {
    x = x + 1;
}

void swap(int &a, int &b) {
    int t = a;
    a = b;
    b = t;
}

void fwd(int &q) {
    inc(q);
}

void show(const int &v) {
    printf("%d\n", v);
}

int byval(int v) {
    return v;
}

int main() {
    int a = 1;
    int b = 2;
    int &r = a;
    int arr[2];
    late(a);
    inc(a);
    inc(r);
    swap(a, b);
    fwd(b);
    inc(arr[1]);
    show(5);
    int n = byval(r);
    scanf("%d", &r);
    return a + b + n;
}

void late(int &x) {
    x++;
}
