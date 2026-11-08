// Feature: if-else (simple if, if-else, else-if ladder, nested)
int classify(int n) {
    int r = 0;
    if (n > 0) {
        r = 1;
    }
    if (n % 2 == 0) {
        r = r + 10;
    } else {
        r = r + 20;
    }
    if (n < 10) {
        r = r + 100;
    } else if (n < 100) {
        r = r + 200;
    } else {
        r = r + 300;
    }
    if (n > 0) {
        if (n > 50) {
            r = r * 2;
        } else {
            r = r * 3;
        }
    }
    return r;
}

int main() {
    int x = classify(42);
    return x;
}
