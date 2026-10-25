// EXPECT: invalid use of 'this'
int main() {
    int *p = this;
    return 0;
}
