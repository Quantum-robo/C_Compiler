// EXPECT: duplicate label 'again'
int main() {
    int a = 0;
again:
    a = 1;
again:
    a = 2;
    return a;
}
