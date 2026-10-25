int main() {
    int i = 0;
    int total = 0;

    while (i < 10) {
        i++;
        if (i == 3) {
            continue;
        }
        if (i == 8) {
            break;
        }
        total += i;
    }

    if (total > 10) {
        goto finish;
    }
    total = 0;

finish:
    return total;
}
