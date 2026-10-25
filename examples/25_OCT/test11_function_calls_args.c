// Feature: function call with arguments (multiple params, nested calls, void fn, expression args)
int add(int a, int b) {
    return a + b;
}

int mul3(int a, int b, int c) {
    return a * b * c;
}

void show(int v) {
    printf("%d\n", v);
    return;
}

int main() {
    int x = add(2, 3);
    int y = mul3(x, x + 1, 2);
    int z = add(add(1, 2), mul3(1, 2, 3));
    show(z);
    show(add(x, y));
    return 0;
}
