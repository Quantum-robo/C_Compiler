int myStrlen(char *s) {
    int n = 0;
    while (*s != 0) {
        n++;
        s++;
    }
    return n;
}
void reverse(char s[]) {
    int i = 0;
    int j = myStrlen(s) - 1;
    while (i < j) {
        char t = s[i];
        s[i] = s[j];
        s[j] = t;
        i++;
        j--;
    }
}
void upper(char s[]) {
    for (int i = 0; s[i] != 0; i++) {
        if (s[i] >= 97 && s[i] <= 122) s[i] = s[i] - 32;
    }
}
int main() {
    char buf[20] = "hello";
    reverse(buf);
    upper(buf);
    printf("%s\n", buf);
    return myStrlen(buf);
}
