// EXPECT: cannot bind non-const reference to an rvalue
void inc(int &x) { x = x + 1; }
int main() {
    inc(5);
    return 0;
}
