// Feature: goto, break and continue (in for, while, do-while, nested loops)
int main() {
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            continue;
        }
        if (i > 7) {
            break;
        }
        sum += i;
    }
    int w = 0;
    while (1) {
        w++;
        if (w == 2) {
            continue;
        }
        if (w >= 5) {
            break;
        }
    }
    int d = 0;
    do {
        d++;
        if (d == 1) {
            continue;
        }
    } while (d < 3);
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            if (a * b == 4) {
                goto found;
            }
        }
    }
    sum = -1;
found:
    return sum + w + d;
}
