// EXPECT: void value not ignored
void f() { return; }
int main() {
    int x = f();
    return x;
}
