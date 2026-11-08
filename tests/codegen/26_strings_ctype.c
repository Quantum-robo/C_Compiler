// String, memory, ctype and number functions of the runtime library
int main() {
    char buf[64];
    char a[16] = "apple";
    char b[16] = "banana";
    strcpy(buf, a);
    strcat(buf, "-");
    strcat(buf, b);
    printf("%s %d\n", buf, strlen(buf));
    printf("%d %d %d\n", strcmp(a, b) < 0, strcmp(b, a) > 0, strcmp(a, "apple"));
    printf("%d %d\n", strncmp("abcdef", "abcxyz", 3), strncmp("abcdef", "abcxyz", 4) < 0);

    char *p = strchr(buf, '-');
    printf("%s %d\n", p, p - buf);
    char path[] = "a/b/c";
    printf("%d %d\n", strrchr(path, '/') - path, strchr(path, 'z') == 0);
    printf("%s|%d\n", strstr(buf, "na"), strstr(buf, "zz") == 0);

    char dst[10];
    memset(dst, 'x', 9);
    dst[9] = 0;
    printf("%s\n", dst);
    memcpy(dst, "abc", 3);
    printf("%s\n", dst);
    memmove(dst + 1, dst, 4);
    printf("%s\n", dst);
    printf("%d %d\n", memcmp("abc", "abd", 3) < 0, memcmp("abc", "abc", 3));
    strncpy(dst, "hi", 5);
    printf("%s %d\n", dst, dst[4]);
    strncat(dst, "xyz12", 3);
    printf("%s\n", dst);

    printf("%d %d %d %d\n", atoi("123"), atoi("  -45x"), atoi("+7"), atoi("abc"));
    printf("%d %d\n", abs(-9), abs(9));

    char s[] = "Hello, World 42!";
    int i, up = 0, lo = 0, di = 0, sp = 0, pu = 0, al = 0, an = 0;
    for (i = 0; s[i] != 0; i++) {
        if (isupper(s[i])) up++;
        if (islower(s[i])) lo++;
        if (isdigit(s[i])) di++;
        if (isspace(s[i])) sp++;
        if (ispunct(s[i])) pu++;
        if (isalpha(s[i])) al++;
        if (isalnum(s[i])) an++;
    }
    printf("%d %d %d %d %d %d %d\n", up, lo, di, sp, pu, al, an);
    for (i = 0; s[i] != 0; i++) printf("%c", toupper(s[i]));
    printf("\n");
    for (i = 0; s[i] != 0; i++) putchar(tolower(s[i]));
    putchar('\n');
    puts("puts adds a newline");
    return 0;
}
