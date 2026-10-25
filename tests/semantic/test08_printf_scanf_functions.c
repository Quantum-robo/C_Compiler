int compute(int x, int y, int z) {
    return (x + y) * z;
}

int main() {
    int a = 0;
    int b = 5;
    scanf("%d", &a);
    int res = compute(a, b, 2);
    printf("Result is: %d\n", res);
    return 0;
}
