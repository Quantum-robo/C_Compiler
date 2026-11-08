// Control-flow edge cases: break / continue inside switch inside loops, empty bodies, goto, deep nesting
int classify(int n) {
    switch (n) {
        case -1: return 100;
        case 0: return 200;
        case 'A': return 300;
        default: return 400;
    }
    return -1;
}

int main() {
    int i, count = 0;
    for (i = 0; i < 6; i++) {
        switch (i % 3) {
            case 0:
                continue;
            case 1:
                count += 10;
                break;
            default:
                count += 1;
        }
        count += 100;
    }
    printf("%d\n", count);

    int n = 0;
    for (i = 0; i < 100; i++) n += i;
    for (i = 0; i < 5; i++);
    printf("%d %d\n", n, i);
    while (0) printf("never\n");
    do n++; while (0);
    printf("%d\n", n);

    switch (7) {
        default:
            printf("only default\n");
    }
    switch (3) {
        case 1: printf("one\n");
        case 2: printf("two\n");
    }
    int v = 2;
    switch (v) {
        case 1: printf("a");
        case 2: printf("b");
        case 3: printf("c");
        case 4: printf("d"); break;
        case 5: printf("e");
    }
    printf("\n");
    printf("%d %d %d %d\n", classify(-1), classify(0), classify(65), classify(9));

    int r, c, found = 0;
    for (r = 0; r < 5; r++) {
        for (c = 0; c < 5; c++) {
            if (c > r) break;
            if ((r + c) % 2) continue;
            found++;
        }
    }
    printf("found=%d\n", found);

    i = 0;
    int total = 0;
again:
    i++;
    if (i % 2 == 0) goto again;
    total += i;
    if (i < 9) goto again;
    printf("total=%d\n", total);

    int depth = 0;
    for (i = 0; i < 2; i++)
        for (r = 0; r < 2; r++)
            for (c = 0; c < 2; c++)
                if (i == r && r == c) {
                    depth++;
                    if (depth == 2) goto leave;
                }
leave:
    printf("depth=%d i=%d\n", depth, i);

    int w = 0;
    while (1) {
        w++;
        switch (w) {
            case 3: break;
            case 5: goto finished;
            default: continue;
        }
        printf("case 3 at %d\n", w);
    }
finished:
    printf("w=%d\n", w);
    return 0;
}
