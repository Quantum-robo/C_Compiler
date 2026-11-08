// Feature: enum and union
enum Color { RED, GREEN = 5, BLUE };
enum Day { MON = 1, TUE, WED };

union Value {
    int i;
    char c;
};

union Bytes {
    int word;
    char b[4];
};

const char *name(enum Color c) {
    switch (c) {
        case RED: return "red";
        case GREEN: return "green";
        case BLUE: return "blue";
    }
    return "?";
}

int main() {
    enum Color c = GREEN;
    printf("%d %d %d\n", RED, GREEN, BLUE);
    printf("%s %s\n", name(c), name(BLUE));
    enum Day d = WED;
    printf("%d\n", d + 1);
    int i;
    for (i = MON; i <= WED; i++) printf("day %d\n", i);
    if (c == GREEN) printf("is green\n");

    union Value v;
    v.i = 65;
    printf("%d %c\n", v.i, v.c);
    v.c = 'z';
    printf("%c\n", v.c);
    union Bytes u;
    u.word = 0;
    u.b[0] = 1;
    u.b[1] = 1;
    printf("%d\n", u.word);
    printf("sizes %d %d\n", sizeof(union Value), sizeof(union Bytes));
    return 0;
}
