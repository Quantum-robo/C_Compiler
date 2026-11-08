// Feature: for loop (decl init, expr init, nested, decrementing, empty parts)
int main() {
    int sum = 0;
    int i;
    for (int k = 0; k < 10; k++) {
        sum = sum + k;
    }
    for (i = 10; i > 0; i--) {
        sum = sum - 1;
    }
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            sum += r * c;
        }
    }
    int j = 0;
    for (; j < 5; ) {
        j += 2;
    }
    return sum;
}
