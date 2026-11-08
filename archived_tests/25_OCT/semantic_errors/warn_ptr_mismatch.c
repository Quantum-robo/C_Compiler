// EXPECT: incompatible pointer types
int main() {
    int x = 1;
    int *p = &x;
    char *q = p;
    return 0;
}
