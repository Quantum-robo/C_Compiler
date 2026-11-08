// Edge cases of floating point: struct copies and returns, array parameters, static and global data, switch on a
// converted value, sprintf, many mixed arguments, nested calls, numerical algorithms.

struct Vec {
    double x, y;
};

struct Vec add(struct Vec a, struct Vec b) {
    struct Vec r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    return r;
}

double dot(struct Vec a, struct Vec b) { return a.x * b.x + a.y * b.y; }

struct Stats {
    int count;
    double sum;
    float low;
    float high;
};

double table[4] = {0.5, 1.5, 2.5, 3.5};
float ftable[2][2] = {{1.5f, 2.5f}, {3.5f, 4.5f}};
double counter;

double next_value() {
    static double state = 0.5;
    state = state * 2 + 0.25;
    return state;
}

void fill(double out[], int n, double start, double step) {
    int i;
    for (i = 0; i < n; i++) out[i] = start + i * step;
}

double sum_all(double *a, int n) {
    double s = 0;
    int i;
    for (i = 0; i < n; i++) s += a[i];
    return s;
}

float lerp(float a, float b, float t) { return a + (b - a) * t; }

double sevenargs(double a, int b, double c, float d, int e, double f, char g) {
    return a + b + c + d + e + f + g;
}

double newton_sqrt(double v) {
    double guess = v / 2;
    int i;
    for (i = 0; i < 20; i++) guess = (guess + v / guess) / 2;
    return guess;
}

double factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int classify(double v) {
    switch ((int)v) {
    case 0: return 100;
    case 1: return 101;
    case 2: return 102;
    default: return -1;
    }
}

void update(struct Stats *s, double v) {
    s->count++;
    s->sum += v;
    if (s->count == 1 || v < s->low) s->low = v;
    if (s->count == 1 || v > s->high) s->high = v;
}

int main() {
    struct Vec p = {1.5, 2.5}, q = {0.5, -1.0};
    struct Vec s = add(p, q);
    struct Vec copy = s;
    copy.x = 10;
    printf("%f %f %f %f %f\n", s.x, s.y, copy.x, dot(p, q), dot(s, s));

    struct Stats st = {0, 0.0, 0.0f, 0.0f};
    double samples[6] = {4.25, -1.5, 9.0, 2.75, 0.0, 6.5};
    int i;
    for (i = 0; i < 6; i++) update(&st, samples[i]);
    printf("%d %f %f %f %f\n", st.count, st.sum, st.low, st.high, st.sum / st.count);

    double buf[5];
    fill(buf, 5, 1.0, 0.25);
    printf("%f %f %f\n", buf[0], buf[4], sum_all(buf, 5));
    printf("%f %f\n", sum_all(table, 4), ftable[1][0] * ftable[0][1]);

    counter += next_value();
    counter += next_value();
    printf("%f %f\n", counter, next_value());

    printf("%f %f\n", lerp(0, 10, 0.25f), lerp(1.5f, 2.5f, 0.5f));
    printf("%f\n", sevenargs(1.5, 2, 3.25, 4.5f, 5, 6.125, 'a'));
    printf("%.6f %.6f\n", newton_sqrt(2), newton_sqrt(144));
    printf("%.1f %.0f %.0f\n", factorial(5), factorial(10), factorial(15));
    printf("%d %d %d %d\n", classify(0.9), classify(1.2), classify(2.99), classify(7.5));

    char out[80];
    sprintf(out, "[%6.2f|%-7.1f|%e]", 3.14159, 2.55, 1234.5);
    printf("%s %d\n", out, (int)strlen(out));

    double acc = 0;
    for (i = 1; i <= 10; i++) acc += 1.0 / i;
    printf("harmonic %.10f\n", acc);
    double x = 0.5, y = 1;
    for (i = 0; i < 5; i++) {
        y = y * x + 1;
        x = x / 3 - 0.125;
    }
    printf("%f %f\n", x, y);

    float big = 16777216.0f;
    big = big + 1;
    printf("%f %f\n", big, 16777216.0 + 1);
    double neg = -0.0;
    double tiny = 1e-300, huge = 1e300;
    printf("%f %d %e %e\n", neg, neg == 0.0, tiny, huge);
    printf("%d %d\n", (int)2147483520.0, (int)-2147483520.0);
    printf("%f %f\n", 1.0 / 3, 2.0 / 3);
    printf("%.15f\n", 0.1 + 0.2);
    printf("%10.4f|%-10.4f|%+10.4f|% .2f\n", -3.14159265, 2.71828, 1.41421, 7.0);
    printf("%g %g %g %g\n", 0.5, 12345678.0, 1e-7, 100.0);

    char c = 100;
    double dc = c;
    short sh = -5;
    double ds = sh;
    long lg = 123456;
    double dl = lg;
    printf("%f %f %f\n", dc, ds, dl);
    bool flag = 0.25;
    bool unset = 0.0;
    printf("%d %d\n", flag, unset);
    return 0;
}
