int main() {
    int sum = 0;

    for (int i = 0; i < 5; i++) {
        sum = sum + i;
    }

    int w = 5;
    while (w > 0) {
        sum += w;
        w--;
    }

    int d = 0;
    do {
        d++;
        sum += d;
    } while (d < 3);

    return sum;
}
