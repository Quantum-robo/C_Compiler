// Feature: references (variables, parameters, returned references, references to struct / array / pointer)
struct P { int x; int y; };

void inc(int &r) { r++; }
void swap(int &a, int &b) { int t = a; a = b; b = t; }
int &larger(int &a, int &b) {
    if (a > b) return a;
    return b;
}
void setx(struct P &p, int v) { p.x = v; }
int sum3(int (&a)[3]) { return a[0] + a[1] + a[2]; }
void reset(int *&p, int *q) { p = q; }

int global = 1;
int &getg() { return global; }

int main() {
    int a = 1, b = 2;
    int &r = a;
    r = 10;
    printf("%d %d\n", a, r);
    r++;
    printf("%d\n", a);

    inc(a);
    inc(b);
    printf("%d %d\n", a, b);
    swap(a, b);
    printf("%d %d\n", a, b);

    larger(a, b) = 0;
    printf("%d %d\n", a, b);

    getg() = 5;
    int &g = getg();
    g += 2;
    printf("%d\n", global);

    struct P pt;
    pt.x = 1; pt.y = 2;
    struct P &rp = pt;
    rp.y = 20;
    setx(pt, 9);
    printf("%d %d\n", pt.x, pt.y);

    int arr[3] = {1, 2, 3};
    int (&ra)[3] = arr;
    ra[1] = 50;
    printf("%d %d\n", sum3(arr), arr[1]);

    int v1 = 1, v2 = 2;
    int *p = &v1;
    int *&rpp = p;
    *rpp = 11;
    reset(p, &v2);
    *p = 22;
    printf("%d %d\n", v1, v2);

    const int &cr = 5;
    printf("%d\n", cr + 1);
    return 0;
}
