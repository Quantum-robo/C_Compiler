#include "MipsGen.hpp"

// Runtime library
// ---------------
// Almost all of it is written in the C subset this compiler accepts (kRuntimeSource) and is compiled with
// the user's program: the code generator emits only the functions the program actually uses, under their own
// label prefix ("RF_"), so a program may define its own strlen, abs, ... without a clash.
//
// Only the four operating-system calls below are hand-written. They follow the compiler's calling
// convention: argument 0 is at 0($sp) on entry, the result is returned in $v0.
const char* const kSysStubsAsm = R"ASM(
# ---- system call stubs used by the runtime library ----
rt_sys_putchar:                 # void __sys_putchar(int c)
    lw   $a0, 0($sp)
    li   $v0, 11
    syscall
    jr   $ra

rt_sys_readline:                # void __sys_readline(char *buf, int n): reads a line; buf[0] is 0 at end of input
    lw   $a0, 0($sp)
    lw   $a1, 4($sp)
    li   $v0, 8
    syscall
    jr   $ra

rt_sys_sbrk:                    # char *__sys_sbrk(int bytes): grows the heap
    lw   $a0, 0($sp)
    li   $v0, 9
    syscall
    jr   $ra

rt_sys_sqrt:                    # double __sys_sqrt(double x): the argument is at 0($sp), the result in $f0
    lwc1 $f0, 0($sp)
    lwc1 $f1, 4($sp)
    sqrt.d $f0, $f0
    jr   $ra

rt_sys_exit:                    # void __sys_exit(int status)
    lw   $a0, 0($sp)
    li   $v0, 17
    syscall
)ASM";

// The library proper. Rules for this source: it is compiled by this compiler (no variadic functions); a variadic function takes its first parameter normally and finds the other
// arguments on the stack behind it; a function is defined before it is used.
const char* const kRuntimeSource = R"RTC(
void __sys_putchar(int c);
void __sys_readline(char *buf, int n);
char *__sys_sbrk(int n);
void __sys_exit(int code);
double __sys_sqrt(double x);

/* ------------------------------ characters ------------------------------ */

int isdigit(int c) { return c >= '0' && c <= '9'; }
int isupper(int c) { return c >= 'A' && c <= 'Z'; }
int islower(int c) { return c >= 'a' && c <= 'z'; }
int isalpha(int c) { return isupper(c) || islower(c); }
int isalnum(int c) { return isalpha(c) || isdigit(c); }
int isspace(int c) { return c == ' ' || (c >= 9 && c <= 13); }
int isprint(int c) { return c >= 32 && c < 127; }
int ispunct(int c) { return isprint(c) && c != ' ' && !isalnum(c); }
int isxdigit(int c) { return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
int toupper(int c) { return islower(c) ? c - 32 : c; }
int tolower(int c) { return isupper(c) ? c + 32 : c; }

/* ---------------------------- strings, memory ---------------------------- */

int strlen(char *s) {
    int n = 0;
    while (s[n] != 0) n++;
    return n;
}

char *strcpy(char *d, char *s) {
    int i = 0;
    while (s[i] != 0) {
        d[i] = s[i];
        i++;
    }
    d[i] = 0;
    return d;
}

char *strncpy(char *d, char *s, int n) {
    int i = 0;
    while (i < n && s[i] != 0) {
        d[i] = s[i];
        i++;
    }
    while (i < n) {
        d[i] = 0;
        i++;
    }
    return d;
}

char *strcat(char *d, char *s) {
    strcpy(d + strlen(d), s);
    return d;
}

char *strncat(char *d, char *s, int n) {
    int len = strlen(d);
    int i = 0;
    while (i < n && s[i] != 0) {
        d[len + i] = s[i];
        i++;
    }
    d[len + i] = 0;
    return d;
}

int strcmp(char *a, char *b) {
    int i = 0;
    while (a[i] != 0 && a[i] == b[i]) i++;
    return a[i] - b[i];
}

int strncmp(char *a, char *b, int n) {
    int i = 0;
    if (n <= 0) return 0;
    while (i < n - 1 && a[i] != 0 && a[i] == b[i]) i++;
    return a[i] - b[i];
}

char *strchr(char *s, int c) {
    while (*s != 0) {
        if (*s == c) return s;
        s++;
    }
    if (c == 0) return s;
    return 0;
}

char *strrchr(char *s, int c) {
    char *found = 0;
    while (*s != 0) {
        if (*s == c) found = s;
        s++;
    }
    if (c == 0) return s;
    return found;
}

char *strstr(char *hay, char *needle) {
    int n = strlen(needle);
    if (n == 0) return hay;
    while (*hay != 0) {
        if (strncmp(hay, needle, n) == 0) return hay;
        hay++;
    }
    return 0;
}

char *memset(char *d, int c, int n) {
    int i;
    for (i = 0; i < n; i++) d[i] = c;
    return d;
}

char *memcpy(char *d, char *s, int n) {
    int i;
    for (i = 0; i < n; i++) d[i] = s[i];
    return d;
}

char *memmove(char *d, char *s, int n) {
    int i;
    if (d < s) {
        for (i = 0; i < n; i++) d[i] = s[i];
    } else {
        for (i = n - 1; i >= 0; i--) d[i] = s[i];
    }
    return d;
}

int memcmp(char *a, char *b, int n) {
    int i;
    for (i = 0; i < n; i++)
        if (a[i] != b[i]) return a[i] - b[i];
    return 0;
}

/* ------------------------------- numbers -------------------------------- */

int abs(int n) { return n < 0 ? -n : n; }
int labs(int n) { return n < 0 ? -n : n; }

int atoi(char *s) {
    int i = 0, sign = 1, v = 0;
    while (isspace(s[i])) i++;
    if (s[i] == '-') {
        sign = -1;
        i++;
    } else if (s[i] == '+') {
        i++;
    }
    while (isdigit(s[i])) {
        v = v * 10 + (s[i] - '0');
        i++;
    }
    return sign * v;
}

int rt_seed = 1;

void srand(int seed) { rt_seed = seed; }

int rand() {
    rt_seed = rt_seed * 1103515245 + 12345;
    return (rt_seed >> 16) & 32767;
}

void exit(int code) { __sys_exit(code); }

/* ---------------------------------- heap --------------------------------- */
/* Every block has an 8-byte header { size of the payload, next free block }.
   Free blocks form a list sorted by address; neighbours are merged when a block is freed. */

struct RtBlock {
    int size;
    struct RtBlock *next;
};

struct RtBlock *rt_free_list = 0;

char *rt_payload(struct RtBlock *b) {
    void *v = b;
    char *c = v;
    return c + 8;
}

char *rt_addr(struct RtBlock *b) {
    void *v = b;
    char *c = v;
    return c;
}

struct RtBlock *rt_header(char *payload) {
    char *p = payload - 8;
    void *v = p;
    struct RtBlock *b = v;
    return b;
}

char *malloc(int n) {
    if (n < 1) n = 1;
    n = (n + 7) / 8 * 8;
    struct RtBlock *prev = 0;
    struct RtBlock *b = rt_free_list;
    while (b != 0) {
        if (b->size >= n) {
            if (b->size >= n + 16) {
                /* split: the tail of the block stays free */
                struct RtBlock *rest = rt_header(rt_payload(b) + n + 8);
                rest->size = b->size - n - 8;
                rest->next = b->next;
                b->size = n;
                b->next = rest;
            }
            if (prev == 0) rt_free_list = b->next;
            else prev->next = b->next;
            return rt_payload(b);
        }
        prev = b;
        b = b->next;
    }
    char *mem = __sys_sbrk(n + 8);
    struct RtBlock *nb = rt_header(mem + 8);
    nb->size = n;
    nb->next = 0;
    return mem + 8;
}

void free(char *p) {
    if (p == 0) return;
    struct RtBlock *b = rt_header(p);
    struct RtBlock *prev = 0;
    struct RtBlock *cur = rt_free_list;
    while (cur != 0 && cur < b) {
        prev = cur;
        cur = cur->next;
    }
    b->next = cur;
    if (prev == 0) rt_free_list = b;
    else prev->next = b;
    if (cur != 0 && rt_addr(cur) == rt_payload(b) + b->size) {
        b->size = b->size + 8 + cur->size;
        b->next = cur->next;
    }
    if (prev != 0 && rt_addr(b) == rt_payload(prev) + prev->size) {
        prev->size = prev->size + 8 + b->size;
        prev->next = b->next;
    }
}

char *calloc(int count, int size) {
    int bytes = count * size;
    char *p = malloc(bytes);
    memset(p, 0, bytes);
    return p;
}

char *realloc(char *p, int n) {
    if (p == 0) return malloc(n);
    int old = rt_header(p)->size;
    if (old >= n) return p;
    char *q = malloc(n);
    memcpy(q, p, old);
    free(p);
    return q;
}

/* ------------------------------ console output --------------------------- */

int putchar(int c) {
    __sys_putchar(c);
    return c;
}

int puts(char *s) {
    int i = 0;
    while (s[i] != 0) {
        __sys_putchar(s[i]);
        i++;
    }
    __sys_putchar(10);
    return 0;
}

/* Output goes to the console, or into a buffer while sprintf / snprintf run. */
char *rt_sink = 0;
int rt_limit = -1;

void rt_put(int c) {
    if (rt_sink == 0) {
        __sys_putchar(c);
        return;
    }
    if (rt_limit == 0) return;
    *rt_sink = c;
    rt_sink++;
    if (rt_limit > 0) rt_limit--;
}

void rt_pad(int n, int c) {
    while (n > 0) {
        rt_put(c);
        n--;
    }
}

/* Prints len characters of s, an optional sign character and padding; returns the number of characters. */
int rt_emit(char *s, int len, int sign, int width, int left, int zero) {
    int total = len;
    int pad = 0;
    int i;
    if (sign != 0) total++;
    if (width > total) pad = width - total;
    if (!left && !zero) rt_pad(pad, ' ');
    if (sign != 0) rt_put(sign);
    if (!left && zero) rt_pad(pad, '0');
    for (i = 0; i < len; i++) rt_put(s[i]);
    if (left) rt_pad(pad, ' ');
    return total + pad;
}

/* v / d with v read as an unsigned number; d must be even (8, 10, 16). */
int rt_udiv(int v, int d) {
    if (v >= 0) return v / d;
    return ((v >> 1) & 2147483647) / (d / 2);
}

/* The digits of v (an unsigned number) in the given base, most significant first; returns how many. */
int rt_digits(int v, int base, int upper, char *out) {
    char tmp[34];
    int n = 0;
    int i;
    do {
        int q = rt_udiv(v, base);
        int r = v - q * base;
        if (r < 10) tmp[n] = '0' + r;
        else if (upper) tmp[n] = 'A' + r - 10;
        else tmp[n] = 'a' + r - 10;
        n++;
        v = q;
    } while (v != 0);
    for (i = 0; i < n; i++) out[i] = tmp[n - 1 - i];
    return n;
}

/* ------------------------------ floating point -------------------------- */

double fabs(double x) {
    if (x < 0.0) return -x;
    return x;
}

double rt_pow10(int n) {
    double r = 1.0;
    double b = 10.0;
    while (n > 0) {
        if (n & 1) r = r * b;
        b = b * b;
        n = n >> 1;
    }
    return r;
}

/* x with the fraction cut off (toward zero); works beyond the range of int */
double rt_trunc(double x) {
    if (x < 0.0) return -rt_trunc(-x);
    if (x < 2147483647.0) return (double)(int)x;
    if (x >= 4503599627370496.0) return x;
    double hi = rt_trunc(x / 1073741824.0);
    return hi * 1073741824.0 + (double)(int)(x - hi * 1073741824.0);
}

double floor(double x) {
    double t = rt_trunc(x);
    if (t > x) return t - 1.0;
    return t;
}

double ceil(double x) {
    double t = rt_trunc(x);
    if (t < x) return t + 1.0;
    return t;
}

double round(double x) {
    if (x < 0.0) return -floor(-x + 0.5);
    return floor(x + 0.5);
}

double fmod(double a, double b) {
    if (b == 0.0) return 0.0;
    return a - rt_trunc(a / b) * b;
}

double sqrt(double x) { return __sys_sqrt(x); }

double exp(double x) {
    int k = (int)floor(x / 0.6931471805599453 + 0.5);
    double r = x - k * 0.6931471805599453;
    double term = 1.0;
    double sum = 1.0;
    int i;
    for (i = 1; i < 25; i++) {
        term = term * r / i;
        sum = sum + term;
    }
    while (k > 0) {
        sum = sum * 2.0;
        k--;
    }
    while (k < 0) {
        sum = sum / 2.0;
        k++;
    }
    return sum;
}

double log(double x) {
    int k = 0;
    int i;
    double y, s2, term, sum;
    if (x <= 0.0) return -1.0e308 * 10.0;
    while (x >= 2.0) {
        x = x / 2.0;
        k++;
    }
    while (x < 1.0) {
        x = x * 2.0;
        k--;
    }
    y = (x - 1.0) / (x + 1.0);
    s2 = y * y;
    term = y;
    sum = 0.0;
    for (i = 1; i < 60; i += 2) {
        sum = sum + term / i;
        term = term * s2;
    }
    return 2.0 * sum + k * 0.6931471805599453;
}

double log10(double x) { return log(x) / 2.302585092994046; }

double pow(double x, double y) {
    int n = (int)y;
    if ((double)n == y) {
        double r = 1.0;
        double b = x;
        int neg = 0;
        if (n < 0) {
            neg = 1;
            n = -n;
        }
        while (n > 0) {
            if (n & 1) r = r * b;
            b = b * b;
            n = n >> 1;
        }
        if (neg) return 1.0 / r;
        return r;
    }
    if (x <= 0.0) return 0.0;
    return exp(y * log(x));
}

/* x brought into [-pi, pi] */
double rt_reduce(double x) {
    double twopi = 6.283185307179586;
    x = x - rt_trunc(x / twopi) * twopi;
    if (x > 3.141592653589793) x = x - twopi;
    if (x < -3.141592653589793) x = x + twopi;
    return x;
}

double sin(double x) {
    double x2, term, sum;
    int i;
    x = rt_reduce(x);
    x2 = x * x;
    term = x;
    sum = x;
    for (i = 1; i < 14; i++) {
        term = -term * x2 / ((2 * i) * (2 * i + 1));
        sum = sum + term;
    }
    return sum;
}

double cos(double x) {
    double x2, term, sum;
    int i;
    x = rt_reduce(x);
    x2 = x * x;
    term = 1.0;
    sum = 1.0;
    for (i = 1; i < 14; i++) {
        term = -term * x2 / ((2 * i - 1) * (2 * i));
        sum = sum + term;
    }
    return sum;
}

double tan(double x) { return sin(x) / cos(x); }

/* strtod / atof: sign, digits, '.', digits, e[+-]digits; end (if not 0) is set past the number */
double strtod(char *s, char **end) {
    double v = 0.0;
    int neg = 0, digits = 0, frac = 0, e = 0, eneg = 0;
    char *p = s;
    while (isspace(*p)) p++;
    if (*p == '-' || *p == '+') {
        neg = *p == '-';
        p++;
    }
    while (isdigit(*p)) {
        v = v * 10.0 + (*p - '0');
        p++;
        digits++;
    }
    if (*p == '.') {
        p++;
        while (isdigit(*p)) {
            v = v * 10.0 + (*p - '0');
            p++;
            frac++;
            digits++;
        }
    }
    if (digits == 0) {
        if (end != 0) *end = s;
        return 0.0;
    }
    if (*p == 'e' || *p == 'E') {
        char *q = p + 1;
        if (*q == '-' || *q == '+') {
            eneg = *q == '-';
            q++;
        }
        if (isdigit(*q)) {
            while (isdigit(*q)) {
                e = e * 10 + (*q - '0');
                q++;
            }
            p = q;
            if (eneg) e = -e;
        }
    }
    e = e - frac;
    if (e < 0) v = v / rt_pow10(-e);
    else v = v * rt_pow10(e);
    if (neg) v = -v;
    if (end != 0) *end = p;
    return v;
}

double atof(char *s) { return strtod(s, 0); }

/* The error of the product p = a * b as rounded to a double (Dekker): a * b = p + error, exactly. */
double rt_prod_err(double a, double b, double p) {
    double c = 134217729.0;
    double t = c * a;
    double ah = t - (t - a);
    double al = a - ah;
    double bh, bl;
    t = c * b;
    bh = t - (t - b);
    bl = b - bh;
    return ((ah * bh - p) + ah * bl + al * bh) + al * bl;
}

/* The digits of x (finite, not negative) with prec decimals, rounded to nearest (ties to even); returns the length */
int rt_fixed(double x, int prec, char *out) {
    char tmp[340];
    double ip = rt_trunc(x);
    double scale, scaled, rs, diff, q;
    int n = 0, i, d, len;
    if (prec > 30) prec = 30;
    scale = rt_pow10(prec);
    scaled = (x - ip) * scale;
    rs = rt_trunc(scaled);
    diff = scaled - rs;
    if (diff > 0.5) {
        rs = rs + 1.0;
    } else if (diff == 0.5) {                    /* a tie only if the product was exact; otherwise look at its error */
        double err = rt_prod_err(x - ip, scale, scaled);
        double last = prec == 0 ? ip : rs;           /* the digit that decides "to even" */
        if (err > 0.0 || (err == 0.0 && rt_trunc(last / 2.0) * 2.0 != last)) rs = rs + 1.0;
    }
    if (rs >= scale) {
        rs = rs - scale;
        ip = ip + 1.0;
    }
    if (ip < 1.0) {
        tmp[n] = '0';
        n++;
    }
    while (ip >= 1.0) {
        q = rt_trunc(ip / 10.0);
        d = (int)(ip - q * 10.0);
        tmp[n] = '0' + d;
        n++;
        ip = q;
    }
    for (i = 0; i < n; i++) out[i] = tmp[n - 1 - i];
    len = n;
    if (prec > 0) {
        out[len] = '.';
        len++;
        for (i = prec - 1; i >= 0; i--) {
            q = rt_trunc(rs / 10.0);
            d = (int)(rs - q * 10.0);
            out[len + i] = '0' + d;
            rs = q;
        }
        len = len + prec;
    }
    return len;
}

/* d.ddde+XX with prec decimals; returns the length */
int rt_expo(double x, int prec, int upper, char *out) {
    int e = 0, len, ex;
    double m = x;
    if (x != 0.0) {
        if (x >= 1.0) {
            while (x >= rt_pow10(e + 1)) e++;
            m = x / rt_pow10(e);
        } else {
            while (x * rt_pow10(-e) < 1.0) e--;
            m = x * rt_pow10(-e);
        }
    }
    len = rt_fixed(m, prec, out);
    if (len - (prec > 0 ? prec + 1 : 0) > 1) {                 /* rounded up to 10.0 */
        e++;
        len = rt_fixed(1.0, prec, out);
    }
    out[len] = upper ? 'E' : 'e';
    len++;
    if (e < 0) {
        out[len] = '-';
        e = -e;
    } else {
        out[len] = '+';
    }
    len++;
    ex = e;
    if (ex >= 100) {
        out[len] = '0' + ex / 100;
        len++;
        ex = ex % 100;
    }
    out[len] = '0' + ex / 10;
    out[len + 1] = '0' + ex % 10;
    return len + 2;
}

/* %g: the shorter of %e and %f style for prec significant digits, without trailing zeros */
int rt_general(double x, int prec, int upper, char *out) {
    int P = prec;
    int len, i, at, j, dot, X, eneg;
    if (P < 0) P = 6;
    if (P == 0) P = 1;
    len = rt_expo(x, P - 1, upper, out);
    at = 0;
    while (out[at] != 'e' && out[at] != 'E') at++;
    X = 0;
    eneg = out[at + 1] == '-';
    for (i = at + 2; i < len; i++) X = X * 10 + (out[i] - '0');
    if (eneg) X = -X;
    if (X < -4 || X >= P) {
        dot = 0;
        for (i = 0; i < at; i++) if (out[i] == '.') dot = 1;
        j = at;
        if (dot) {
            while (out[j - 1] == '0') j--;
            if (out[j - 1] == '.') j--;
        }
        for (i = at; i < len; i++) {
            out[j] = out[i];
            j++;
        }
        return j;
    }
    len = rt_fixed(x, P - 1 - X, out);
    dot = 0;
    for (i = 0; i < len; i++) if (out[i] == '.') dot = 1;
    if (dot) {
        while (out[len - 1] == '0') len--;
        if (out[len - 1] == '.') len--;
    }
    return len;
}

/* The formatting engine behind printf, sprintf and snprintf: %d %i %u %x %X %o %c %s %p %f %e %g %% with flags - 0 + space,
   a width (or *) and .precision. "ap" points at the first argument; returns the number of characters. */
int rt_format(char *fmt, void *ap) {
    int count = 0;
    while (*fmt != 0) {
        if (*fmt != '%') {
            rt_put(*fmt);
            fmt++;
            count++;
            continue;
        }
        fmt++;
        int left = 0, zero = 0, plus = 0, space = 0, width = 0, prec = -1;
        while (*fmt == '-' || *fmt == '0' || *fmt == '+' || *fmt == ' ') {
            if (*fmt == '-') left = 1;
            else if (*fmt == '0') zero = 1;
            else if (*fmt == '+') plus = 1;
            else space = 1;
            fmt++;
        }
        if (*fmt == '*') {
            int *wp = ap;
            width = *wp;
            ap = ap + 4;
            if (width < 0) {
                left = 1;
                width = -width;
            }
            fmt++;
        }
        while (isdigit(*fmt)) {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }
        if (*fmt == '.') {
            fmt++;
            prec = 0;
            while (isdigit(*fmt)) {
                prec = prec * 10 + (*fmt - '0');
                fmt++;
            }
        }
        while (*fmt == 'l' || *fmt == 'h' || *fmt == 'L') fmt++;
        int spec = *fmt;
        if (spec == 0) break;
        fmt++;
        if (spec == '%') {
            rt_put('%');
            count++;
            continue;
        }
        void *slot = ap;
        ap = ap + 4;
        char buf[40];
        if (spec == 'd' || spec == 'i') {
            int *ip = slot;
            int v = *ip;
            int sign = 0;
            if (v < 0) {
                sign = '-';
                v = -v;
            } else if (plus) {
                sign = '+';
            } else if (space) {
                sign = ' ';
            }
            if (left) zero = 0;
            count += rt_emit(buf, rt_digits(v, 10, 0, buf), sign, width, left, zero);
        } else if (spec == 'u' || spec == 'x' || spec == 'X' || spec == 'o') {
            int *ip = slot;
            int base = 10;
            if (spec == 'x' || spec == 'X') base = 16;
            if (spec == 'o') base = 8;
            if (left) zero = 0;
            count += rt_emit(buf, rt_digits(*ip, base, spec == 'X', buf), 0, width, left, zero);
        } else if (spec == 'p') {
            int *ip = slot;
            rt_put('0');
            rt_put('x');
            count += 2 + rt_emit(buf, rt_digits(*ip, 16, 0, buf), 0, width, left, 0);
        } else if (spec == 'c') {
            int *ip = slot;
            buf[0] = *ip;
            count += rt_emit(buf, 1, 0, width, left, 0);
        } else if (spec == 'f' || spec == 'F' || spec == 'e' || spec == 'E' || spec == 'g' || spec == 'G') {
            double *dp = slot;
            double v = *dp;
            int sign = 0, len, p = prec;
            char fb[360];
            ap = ap + 4;                                  /* a double takes two words */
            if (p < 0) p = 6;
            if (v < 0.0 || (v == 0.0 && 1.0 / v < 0.0)) {     /* also -0.0 */
                sign = '-';
                v = -v;
            } else if (plus) {
                sign = '+';
            } else if (space) {
                sign = ' ';
            }
            if (v != v) {
                fb[0] = 'n';
                fb[1] = 'a';
                fb[2] = 'n';
                len = 3;
                zero = 0;
            } else if (v - v != 0.0) {
                fb[0] = 'i';
                fb[1] = 'n';
                fb[2] = 'f';
                len = 3;
                zero = 0;
            } else if (spec == 'f' || spec == 'F') {
                len = rt_fixed(v, p, fb);
            } else if (spec == 'e' || spec == 'E') {
                len = rt_expo(v, p, spec == 'E', fb);
            } else {
                len = rt_general(v, prec, spec == 'G', fb);
            }
            if (left) zero = 0;
            count += rt_emit(fb, len, sign, width, left, zero);
        } else if (spec == 's') {
            char **sp = slot;
            char *s = *sp;
            int len;
            if (s == 0) s = "(null)";
            len = strlen(s);
            if (prec >= 0 && prec < len) len = prec;
            count += rt_emit(s, len, 0, width, left, 0);
        } else {
            rt_put('%');
            rt_put(spec);
            count += 2;
        }
    }
    return count;
}

int printf(char *fmt) {
    void *ap = &fmt;
    ap = ap + 4;
    return rt_format(fmt, ap);
}

int sprintf(char *buf, char *fmt) {
    void *ap = &fmt;
    int n;
    ap = ap + 4;
    rt_sink = buf;
    rt_limit = -1;
    n = rt_format(fmt, ap);
    *rt_sink = 0;
    rt_sink = 0;
    return n;
}

int snprintf(char *buf, int size, char *fmt) {
    void *ap = &fmt;
    int n;
    ap = ap + 4;
    rt_sink = buf;
    rt_limit = size > 0 ? size - 1 : 0;
    n = rt_format(fmt, ap);
    if (size > 0) *rt_sink = 0;
    rt_sink = 0;
    return n;
}

/* ------------------------------ console input ---------------------------- */
/* Input is read a line at a time; getchar returns -1 at the end of the input. */

char rt_line[260];
int rt_pos = 0;
int rt_len = 0;

int getchar() {
    int c;
    if (rt_pos >= rt_len) {
        __sys_readline(rt_line, 256);
        if (rt_line[0] == 0) return -1;
        rt_len = strlen(rt_line);
        rt_pos = 0;
    }
    c = rt_line[rt_pos];
    rt_pos++;
    return c;
}

/* gives back the character just read (one character of look-ahead is enough) */
void rt_ungetc(int c) {
    if (c >= 0 && rt_pos > 0) rt_pos--;
}

char *gets(char *buf) {
    int i = 0;
    int c = getchar();
    if (c < 0) return 0;
    while (c >= 0 && c != 10) {
        buf[i] = c;
        i++;
        c = getchar();
    }
    buf[i] = 0;
    return buf;
}

/* scanf(format, pointers...): %d %i %u %x %o %c %s %% with an optional width, h / l modifiers and %*d;
   returns the number of values stored, or -1 if the input ended before the first conversion */
int scanf(char *fmt) {
    void *ap = &fmt;
    ap = ap + 4;
    int assigned = 0;
    int c;
    while (*fmt != 0) {
        if (isspace(*fmt)) {
            fmt++;
            c = getchar();
            while (c >= 0 && isspace(c)) c = getchar();
            rt_ungetc(c);
            continue;
        }
        if (*fmt != '%') {
            c = getchar();
            if (c != *fmt) {
                rt_ungetc(c);
                if (c < 0 && assigned == 0) return -1;
                return assigned;
            }
            fmt++;
            continue;
        }
        fmt++;
        int suppress = 0, width = 0, half = 0, lng = 0;
        if (*fmt == '*') {
            suppress = 1;
            fmt++;
        }
        while (isdigit(*fmt)) {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }
        while (*fmt == 'l' || *fmt == 'h') {
            if (*fmt == 'h') half = 1;
            else lng = 1;
            fmt++;
        }
        int spec = *fmt;
        if (spec == 0) break;
        fmt++;
        if (spec == '%') {
            c = getchar();
            if (c != '%') {
                rt_ungetc(c);
                break;
            }
            continue;
        }
        if (spec == 'c') {
            c = getchar();
            if (c < 0) {
                if (assigned == 0) return -1;
                return assigned;
            }
            if (!suppress) {
                void *slot = ap;
                char **dp = slot;
                char *d = *dp;
                ap = ap + 4;
                *d = c;
                assigned++;
            }
            continue;
        }
        /* every other conversion skips white space first */
        c = getchar();
        while (c >= 0 && isspace(c)) c = getchar();
        if (c < 0) {
            if (assigned == 0) return -1;
            return assigned;
        }
        if (spec == 's') {
            char *d = 0;
            int len = 0;
            if (!suppress) {
                void *slot = ap;
                char **dp = slot;
                d = *dp;
                ap = ap + 4;
            }
            while (c >= 0 && !isspace(c) && (width == 0 || len < width)) {
                if (!suppress) d[len] = c;
                len++;
                c = getchar();
            }
            rt_ungetc(c);
            if (!suppress) {
                d[len] = 0;
                assigned++;
            }
            continue;
        }
        if (spec == 'f' || spec == 'F' || spec == 'e' || spec == 'E' || spec == 'g' || spec == 'G') {
            char nb[64];
            int n = 0, seen = 0;
            if (c == '-' || c == '+') {
                nb[n] = c;
                n++;
                c = getchar();
            }
            while (c >= 0 && isdigit(c) && n < 60 && (width == 0 || n < width)) {
                nb[n] = c;
                n++;
                seen++;
                c = getchar();
            }
            if (c == '.' && n < 60) {
                nb[n] = c;
                n++;
                c = getchar();
                while (c >= 0 && isdigit(c) && n < 60 && (width == 0 || n < width)) {
                    nb[n] = c;
                    n++;
                    seen++;
                    c = getchar();
                }
            }
            if (seen > 0 && (c == 'e' || c == 'E')) {
                nb[n] = c;
                n++;
                c = getchar();
                if (c == '-' || c == '+') {
                    nb[n] = c;
                    n++;
                    c = getchar();
                }
                while (c >= 0 && isdigit(c) && n < 60) {
                    nb[n] = c;
                    n++;
                    c = getchar();
                }
            }
            rt_ungetc(c);
            if (seen == 0) break;
            nb[n] = 0;
            if (!suppress) {
                double v = strtod(nb, 0);
                void *slot = ap;
                ap = ap + 4;
                if (lng) {
                    double **dp = slot;
                    double *d = *dp;
                    *d = v;
                } else {
                    float **fp = slot;
                    float *fl = *fp;
                    *fl = v;
                }
                assigned++;
            }
            continue;
        }
        int base = 10;
        int neg = 0, digits = 0, value = 0, used = 0;
        if (spec == 'x' || spec == 'X') base = 16;
        if (spec == 'o') base = 8;
        if (c == '-' || c == '+') {
            neg = c == '-';
            used++;
            c = getchar();
        }
        while (c >= 0 && (width == 0 || used < width)) {
            int dv;
            if (isdigit(c)) dv = c - '0';
            else if (isalpha(c)) dv = tolower(c) - 'a' + 10;
            else break;
            if (dv >= base) break;
            value = value * base + dv;
            digits++;
            used++;
            c = getchar();
        }
        rt_ungetc(c);
        if (digits == 0) break;
        if (neg) value = -value;
        if (!suppress) {
            void *slot = ap;
            ap = ap + 4;
            if (half) {
                short **hp = slot;
                short *h = *hp;
                *h = value;
            } else {
                int **ip = slot;
                int *d = *ip;
                *d = value;
            }
            assigned++;
        }
    }
    return assigned;
}
)RTC";
