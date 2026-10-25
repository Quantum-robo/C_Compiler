int main() {
    char op = 43;
    int r = 0;
    switch (op) {
        case 43: r = 1; break;
        case 45: r = 2; break;
        default: break;
    }
    int k = 2;
    switch (k) {
        case 1:
        case 2: r += 10; break;
    }
    return r;
}
