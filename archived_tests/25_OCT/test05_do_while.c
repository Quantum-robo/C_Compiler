// Feature: do-while loop (body runs at least once, nested)
int main() {
    int count = 0;
    do {
        count++;
    } while (count < 5);

    int x = 100;
    do {
        x = x - 30;
    } while (x > 0);

    int outer = 0;
    int acc = 0;
    do {
        int inner = 0;
        do {
            acc += outer + inner;
            inner++;
        } while (inner < 2);
        outer++;
    } while (outer < 2);
    return count + acc;
}
