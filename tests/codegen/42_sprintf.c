// sprintf / snprintf, building strings, and main(argc, argv)
int main(int argc, char **argv) {
    char buf[64];
    int n = sprintf(buf, "%d-%s-%c", 42, "abc", 'z');
    printf("%s (%d chars, strlen %d)\n", buf, n, strlen(buf));

    char line[32];
    sprintf(line, "[%5d][%-5s][%05d]", 7, "ab", 42);
    printf("%s\n", line);

    char small[8];
    int want = snprintf(small, 8, "%s", "truncated text");
    printf("%s %d\n", small, want);
    snprintf(small, 8, "%d", 123);
    printf("%s\n", small);

    char name[32] = "item";
    char full[64];
    int i;
    full[0] = 0;
    for (i = 1; i <= 3; i++) {
        char part[16];
        sprintf(part, "%s%d;", name, i);
        strcat(full, part);
    }
    printf("%s\n", full);
    printf("%d\n", argc >= 1 && argv != 0);
    return 0;
}
