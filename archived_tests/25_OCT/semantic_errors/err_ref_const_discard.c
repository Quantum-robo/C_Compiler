// EXPECT: discards qualifiers
int main() {
    const int c = 1;
    int &r = c;
    return 0;
}
