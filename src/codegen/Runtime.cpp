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

rt_sys_exit:                    # void __sys_exit(int status)
    lw   $a0, 0($sp)
    li   $v0, 17
    syscall
)ASM";

// The library proper. Rules for this source: it is compiled by this compiler (no casts, no floating point,
// no variadic functions); a variadic function takes its first parameter normally and finds the other
// arguments on the stack behind it; a function is defined before it is used.
const char* const kRuntimeSource = R"RTC(
void __sys_putchar(int c);
void __sys_readline(char *buf, int n);
char *__sys_sbrk(int n);
void __sys_exit(int code);

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

void rt_pad(int n, int c) {
    while (n > 0) {
        __sys_putchar(c);
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
    if (sign != 0) __sys_putchar(sign);
    if (!left && zero) rt_pad(pad, '0');
    for (i = 0; i < len; i++) __sys_putchar(s[i]);
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

/* printf(format, ...): %d %i %u %x %X %o %c %s %p %% with flags - 0 + space, a width (or *) and .precision for %s */
int printf(char *fmt) {
    void *ap = &fmt;
    ap = ap + 4;
    int count = 0;
    while (*fmt != 0) {
        if (*fmt != '%') {
            __sys_putchar(*fmt);
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
        while (*fmt == 'l' || *fmt == 'h') fmt++;
        int spec = *fmt;
        if (spec == 0) break;
        fmt++;
        if (spec == '%') {
            __sys_putchar('%');
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
            __sys_putchar('0');
            __sys_putchar('x');
            count += 2 + rt_emit(buf, rt_digits(*ip, 16, 0, buf), 0, width, left, 0);
        } else if (spec == 'c') {
            int *ip = slot;
            buf[0] = *ip;
            count += rt_emit(buf, 1, 0, width, left, 0);
        } else if (spec == 's') {
            char **sp = slot;
            char *s = *sp;
            int len;
            if (s == 0) s = "(null)";
            len = strlen(s);
            if (prec >= 0 && prec < len) len = prec;
            count += rt_emit(s, len, 0, width, left, 0);
        } else {
            __sys_putchar('%');
            __sys_putchar(spec);
            count += 2;
        }
    }
    return count;
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
        int suppress = 0, width = 0, half = 0;
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
