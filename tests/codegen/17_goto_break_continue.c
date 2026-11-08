// Feature: goto, break and continue
int main() {
    int i = 0;
loop:
    if (i >= 3) goto done;
    printf("goto loop %d\n", i);
    i++;
    goto loop;
done:
    printf("done\n");

    int n;
    for (n = 0; n < 10; n++) {
        if (n % 2 == 0) continue;
        if (n > 7) break;
        printf("n=%d\n", n);
    }

    int found = -1;
    int r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            if (r * c == 6) {
                found = r * 10 + c;
                goto out;
            }
        }
    }
out:
    printf("found=%d\n", found);

    int k = 0;
    while (1) {
        k++;
        if (k < 3) continue;
        if (k == 5) break;
        printf("k=%d\n", k);
    }
    int x = 0;
    do {
        x++;
        if (x == 2) continue;
        if (x == 4) break;
        printf("x=%d\n", x);
    } while (x < 10);
    return 0;
}
