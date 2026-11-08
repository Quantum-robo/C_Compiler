// EXPECT: called before it was declared
int main() {
    int a = 1;
    later(a);
    return a;
}
void later(int &x) {
    x = 2;
}
