// malloc / free / calloc / realloc: contents stay intact while blocks are freed and the memory is reused
struct Rec {
    int id;
    int data[3];
};

int fill(int *p, int n, int seed) {
    int j;
    for (j = 0; j < n; j++) p[j] = seed * 100 + j;
    return n;
}

int check(int *p, int n, int seed) {
    int j, bad = 0;
    for (j = 0; j < n; j++)
        if (p[j] != seed * 100 + j) bad++;
    return bad;
}

int main() {
    int *blocks[50];
    int sizes[50];
    int i, bad = 0;
    for (i = 0; i < 50; i++) {
        sizes[i] = (i * 7) % 13 + 1;
        blocks[i] = malloc(sizes[i] * sizeof(int));
        fill(blocks[i], sizes[i], i);
    }
    for (i = 0; i < 50; i++) bad += check(blocks[i], sizes[i], i);
    printf("bad=%d\n", bad);

    for (i = 0; i < 50; i += 2) {
        free(blocks[i]);
        blocks[i] = 0;
    }
    for (i = 0; i < 50; i += 2) {
        sizes[i] = (i % 5) + 2;
        blocks[i] = malloc(sizes[i] * sizeof(int));
        fill(blocks[i], sizes[i], i + 1000);
    }
    bad = 0;
    for (i = 0; i < 50; i++) bad += check(blocks[i], sizes[i], i % 2 == 0 ? i + 1000 : i);
    printf("bad after reuse=%d\n", bad);

    int *z = calloc(10, sizeof(int));
    int sum = 0;
    for (i = 0; i < 10; i++) sum += z[i];
    printf("calloc sum=%d\n", sum);

    int *g = malloc(4 * sizeof(int));
    for (i = 0; i < 4; i++) g[i] = i + 1;
    g = realloc(g, 100 * sizeof(int));
    printf("%d %d %d %d\n", g[0], g[1], g[2], g[3]);
    g[99] = 7;
    printf("%d\n", g[99]);

    for (i = 0; i < 1000; i++) {
        int *t = malloc(64);
        t[0] = i;
        t[15] = i * 2;
        if (t[0] + t[15] != i * 3) printf("corrupt %d\n", i);
        free(t);
    }

    struct Rec *recs = malloc(5 * sizeof(struct Rec));
    for (i = 0; i < 5; i++) {
        recs[i].id = i;
        recs[i].data[2] = i * i;
    }
    printf("%d %d\n", recs[4].id, recs[3].data[2]);
    free(recs);

    for (i = 0; i < 50; i++) free(blocks[i]);
    free(z);
    free(g);
    int *last = malloc(3 * sizeof(int));
    last[0] = 1; last[1] = 2; last[2] = 3;
    printf("%d\n", last[0] + last[1] + last[2]);
    free(last);
    return 0;
}
