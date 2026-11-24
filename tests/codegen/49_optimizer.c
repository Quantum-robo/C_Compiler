// Optimizer, end to end: folded and propagated values must equal what the program computes without the optimizer,
// and the cases where optimizing would be wrong (pointers, globals changed by calls, loops, narrow types) stay correct
#include <stdio.h>

int counter = 0;

int bump() {
    counter = counter + 10;
    return counter;
}

int folding(int one) {
    int b = 10;
    int c = b * 4 - 1;
    c = c + (b << 2) + (b >> 1) + (b % 3) + (-b / 4) + (-7 / 2) + (-7 % 2) + (-16 >> 2);
    c = c + (b > 5) + (b == 10) + !b + ~b + -b + (b != 3) + (b <= 9);
    return c + b / one;
}

int copies(int a) {
    int b = a;
    int c = b;
    int d = c + b;
    b = 100;
    return d + c + b;
}

int cse(int w, int h) {
    int a = w * h + 1;
    int b = h * w + 2;
    int c = (w + h) * (w + h);
    w = w + 1;
    int d = w * h;
    return a + b + c + d;
}

int pointers(int x) {
    int y = x;
    int *p = &y;
    *p = 7;
    int z = y + 1;
    *p = z * 2;
    return y + z;
}

int globals() {
    counter = 1;
    int before = counter;
    bump();
    int after = counter;
    return before * 100 + after;
}

int loops(int n) {
    int i = 0;
    int sum = 0;
    int step = 3;
    while (i < n) {
        sum = sum + step;
        step = step + 1;
        i = i + 1;
    }
    return sum;
}

int narrow() {
    char c = 100;
    c = c + 100;
    short s = 30000;
    s = s + 30000;
    int r = c + s;
    return r;
}

int branches(int k) {
    int limit = 4;
    int r = 0;
    if (limit > 3) r = r + 1;
    if (limit < 3) r = r + 100;
    if (k > limit) r = r + 10;
    return r;
}

int main() {
    printf("folding  %d\n", folding(1));
    printf("copies   %d\n", copies(5));
    printf("cse      %d\n", cse(2, 3));
    printf("pointers %d\n", pointers(4));
    printf("globals  %d\n", globals());
    printf("loops    %d\n", loops(5));
    printf("narrow   %d\n", narrow());
    printf("branches %d %d\n", branches(2), branches(9));
    int unused = bump();
    printf("counter  %d\n", counter);
    return 0;
}
