// EXPECT: read-only
int main() {
    const int c = 1;
    c = 2;
    return c;
}
