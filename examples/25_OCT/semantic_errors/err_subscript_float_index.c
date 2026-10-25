// EXPECT: array subscript is not an integer
int main() {
    int a[3];
    int b = a[1.5];
    return b;
}
