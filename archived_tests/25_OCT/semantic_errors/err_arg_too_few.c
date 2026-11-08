// EXPECT: too few arguments to function 'add' (expected 2, got 1)
int add(int a, int b) { return a + b; }
int main() {
    return add(1);
}
