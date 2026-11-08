int len(const char *s) {
    int n = 0;
    while (s[n] != 0) { n++; }
    return n;
}
int main() {
    const int k = 3;
    const char *name = "abc";
    int copy = k;
    int arr[3] = {1, 2, 3};
    const int *cp = &k;
    int v = *cp + len(name);
    return v + copy + arr[k - 1];
}
