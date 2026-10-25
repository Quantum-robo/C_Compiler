// EXPECT: cannot modify array 'a'
int main() {
    int a[3];
    int b[3];
    a = b;
    return 0;
}
