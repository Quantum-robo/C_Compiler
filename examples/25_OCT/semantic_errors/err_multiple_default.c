// EXPECT: multiple default labels in one switch
int main() {
    int a = 1;
    switch (a) {
        default: break;
        default: break;
    }
    return 0;
}
