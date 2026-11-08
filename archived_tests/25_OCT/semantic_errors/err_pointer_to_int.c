// EXPECT: makes integer from pointer without a cast
int main() {
    int v = 1;
    int *p = &v;
    int x = p;
    return x;
}
