// EXPECT: discards 'const' qualifier
int main() {
    const int c = 1;
    int *p = &c;
    return 0;
}
