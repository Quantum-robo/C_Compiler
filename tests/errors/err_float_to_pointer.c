// EXPECT: incompatible types in initialization
int main() {
    double d = 1.5;
    int *p = d;
    return 0;
}
