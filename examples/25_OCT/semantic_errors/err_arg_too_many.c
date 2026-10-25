// EXPECT: too many arguments to function 'add' (expected 2, got 3)
int add(int a, int b) { return a + b; }
int main() {
    return add(1, 2, 3);
}
