// Feature: while loop (simple, nested, compound condition)
int main() {
    int n = 12345;
    int digits = 0;
    while (n > 0) {
        n = n / 10;
        digits++;
    }
    int i = 0;
    int total = 0;
    while (i < 3) {
        int j = 0;
        while (j < 3) {
            total = total + i * j;
            j++;
        }
        i++;
    }
    int a = 0;
    int b = 10;
    while (a < 5 && b > 5) {
        a++;
        b--;
    }
    return digits + total;
}
