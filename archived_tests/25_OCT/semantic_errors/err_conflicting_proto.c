// EXPECT: conflicting declaration of 'f'
int f(int a);
int f(int a, int b) { return a + b; }
int main() { return 0; }
