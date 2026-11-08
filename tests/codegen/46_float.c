// float and double: arithmetic, conversions, comparisons, functions, arrays, structs, classes, printf / scanf %f

double g_total = 1.5;
float g_small = 0.25f;

struct Point {
    double x;
    double y;
    float weight;
    int id;
};

class Circle {
public:
    double radius;
    Circle(double r) : radius(r) { }
    double area() { return 3.14159 * radius * radius; }
    void grow(double by) { radius += by; }
};

double square(double v) { return v * v; }
float half(float v) { return v / 2; }
int to_int(double v) { return (int)v; }
double mix(int a, double b, float c, char d) { return a + b * c - d; }

double power(double base, int n) {
    if (n == 0) return 1.0;
    return base * power(base, n - 1);
}

double average(double *values, int count) {
    double sum = 0;
    int i;
    for (i = 0; i < count; i++) sum += values[i];
    return sum / count;
}

void scale(double &value, double factor) { value = value * factor; }

int main() {
    // arithmetic and literals
    double a = 7.5, b = 2.0;
    printf("%f %f %f %f\n", a + b, a - b, a * b, a / b);
    printf("%.2f %.0f %.3f\n", 3.14159, 2.5, -0.0005);
    printf("%5.1f|%-8.2f|%08.3f|%+.1f\n", 3.14159, 2.5, -1.25, 2.0);

    // int <-> floating conversions
    int n = 7;
    double d = n;                    // 7.0
    double q = n / 2;                // integer division, then converted: 3
    double r = n / 2.0;              // 3.5
    int t = 9.99;                    // truncates to 9
    int neg = -9.99;                 // -9
    printf("%f %f %f %d %d\n", d, q, r, t, neg);
    printf("%d %d %d\n", (int)3.99, (int)-3.99, (int)(a * b));
    printf("%f %f\n", (double)n / 4, (float)n / 4);

    // float is single precision, double is double precision
    float f = 0.1f;
    double df = f;
    printf("%.10f %.10f %.10f\n", f, df, 0.1);
    printf("%d %d %d\n", (int)sizeof(float), (int)sizeof(double), (int)sizeof(struct Point));

    // comparisons and conditions
    double c1 = 0.1 + 0.2, c2 = 0.3;
    printf("%d %d %d %d %d %d\n", c1 == c2, c1 != c2, a < b, a > b, a <= 7.5, b >= 2);
    if (a) printf("a is true\n");
    if (!0.0) printf("0.0 is false\n");
    double zero = 0;
    while (zero < 0.45) zero += 0.1;
    printf("loop ended at %.2f\n", zero);
    printf("%d %d\n", a && b, zero && 0.0);

    // unary operators, ++, --, compound assignment, ternary
    double u = 1.5;
    u++;
    ++u;
    u--;
    printf("%f %f\n", u, -u);
    u *= 3;
    u -= 0.5;
    u /= 4;
    u += n;
    printf("%f\n", u);
    int iv = 10;
    iv += 2.7;                       // computed as a double, truncated: 12
    iv *= 1.5;                       // 18
    printf("%d\n", iv);
    printf("%f %f\n", a > b ? a : b, n > 3 ? 1 : 2.5);

    // functions
    printf("%f %f %d\n", square(1.5), square(3), to_int(square(2.5)));
    printf("%f %f\n", half(5), half(2.5f));
    printf("%f\n", mix(2, 1.5, 2.0f, 'A'));
    printf("%f %f\n", power(2.0, 10), power(1.5, 3));

    // arrays, pointers, references
    double vals[5] = {1.5, 2.5, 3.5, 4.5, 5.5};
    float fvals[3] = {1, 2.5f, 3};
    printf("%f %f %f\n", average(vals, 5), vals[2], fvals[1] + fvals[2]);
    double *p = vals;
    p++;
    *p = 10;
    p[2] += 0.25;
    printf("%f %f %f\n", vals[1], vals[3], *(p + 3));
    double matrix[2][2] = {{1, 2}, {3.5, 4}};
    printf("%f %f\n", matrix[1][0], matrix[0][1] * matrix[1][1]);
    scale(vals[0], 4);
    double w = 2.5;
    double &ref = w;
    ref = ref + 1;
    printf("%f %f\n", vals[0], w);

    // structs, globals, dynamic memory
    struct Point pt = {1.5, -2.25, 0.5f, 3};
    struct Point *pp = &pt;
    pp->x = pp->x * 2;
    pt.weight = pt.weight + pt.y;
    printf("%f %f %f %d\n", pt.x, pt.y, pt.weight, pt.id);
    g_total = g_total + g_small;
    g_total *= 2;
    printf("%f %f\n", g_total, g_small);
    double *heap = new double[3];
    heap[0] = 0.5;
    heap[1] = heap[0] * 3;
    heap[2] = heap[1] / 4;
    printf("%f %f %f\n", heap[0], heap[1], heap[2]);
    delete[] heap;
    double *one = (double *)malloc(sizeof(double));
    *one = 123.456;
    printf("%.3f\n", *one);
    free(one);

    // classes
    Circle c(2.0);
    c.grow(0.5);
    printf("%.4f %.4f\n", c.radius, c.area());

    // library
    printf("%f %f %f %f\n", sqrt(2), sqrt(144.0), fabs(-3.5), pow(2, 10));
    printf("%f %f %f %f\n", floor(-2.5), ceil(-2.5), floor(2.5), ceil(2.1));
    printf("%.3f %.3f %.3f\n", sin(1.0), cos(1.0), pow(2.0, 0.5));
    printf("%.4f %.4f\n", exp(1.0), log(10.0));
    printf("%f %f\n", atof("3.25"), atof("-1.5e2"));

    // %e and %g
    printf("%e %e %.2e\n", 12345.678, 0.000123, 5.0);
    printf("%g %g %g %g %g\n", 100000.0, 1000000.0, 0.0001, 0.00001, 123.456);
    printf("%g %g %.3g\n", 2.5, 3.0, 3.14159);

    // large and small values, rounding
    printf("%f %f\n", 1234567890.125, 0.000001);
    printf("%.1f %.1f %.1f %.1f\n", 0.05, 0.15, 0.25, 0.35);
    printf("%.0f %.0f %.0f %.0f\n", 0.5, 1.5, 2.5, 3.5);
    printf("%f %f\n", 999999.9999999, 1e10);

    // scanf
    float fin;
    double din;
    int iin;
    scanf("%f %lf", &fin, &din);
    scanf("%d", &iin);
    printf("%f %f %d\n", fin, din, iin);
    printf("%f\n", fin * din * iin);
    return 0;
}
