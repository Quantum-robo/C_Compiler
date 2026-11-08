// EXPECT: label 'nowhere' used but not defined
int main() {
    goto nowhere;
    return 0;
}
