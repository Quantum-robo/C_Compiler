// EXPECT: subscripted value is neither array nor pointer
int main() {
    int a = 1;
    int b = a[0];
    return b;
}
