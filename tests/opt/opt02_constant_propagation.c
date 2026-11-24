// Optimizer: constant propagation. x, y and z become known constants, so the condition z > 20 is the constant 1:
// the branch is resolved, the else part becomes unreachable and is removed, and "if (0)" disappears entirely.
#include <stdio.h>

int main() {
    int x = 5;
    int y = x + 1;
    int z = y * x;
    if (z > 20)
        printf("big %d\n", z);
    else
        printf("small\n");
    if (0)
        printf("never\n");
    return z - 30;
}
