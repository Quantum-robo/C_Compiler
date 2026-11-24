// Optimizer: dead code elimination. A value that is never read (unused, the old value saved by k++), code after
// a return, a self-assignment and a call's unused result are removed; the call itself stays (side effects).
#include <stdio.h>

int work(int n) {
    int unused = n * 100;
    int k = n;
    k++;
    k = k;
    printf("%d\n", k);
    return k;
    printf("never printed\n");
    return 0;
}

int main() {
    work(4);
    return 0;
}
