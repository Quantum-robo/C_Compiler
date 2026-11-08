// EXPECT: read-only
int main() {
    const char *s = "x";
    *s = 97;
    return 0;
}
