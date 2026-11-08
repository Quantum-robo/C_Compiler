// EXPECT: duplicate case value '1'
int main() {
    int a = 1;
    switch (a) {
        case 1: break;
        case 1: break;
    }
    return 0;
}
