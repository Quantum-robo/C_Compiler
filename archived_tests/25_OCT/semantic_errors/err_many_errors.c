// EXPECT: SEMANTIC ANALYSIS FAILED: 4 error(s)
int main() {
    a = 1;
    b = 2;
    break;
    int c = 1;
    int c = 2;
    return 0;
}
