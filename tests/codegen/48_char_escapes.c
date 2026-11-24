// Character and string escapes: octal ('\101', '\0', '\12', '\177'), hex ('\x41'), and the simple escapes
#include <stdio.h>

int main() {
    char a = '\101';
    char nul = '\0';
    char nl = '\12';
    char del = '\177';
    char h = '\x41';
    char tab = '\t';
    char q = '\'';
    char bs = '\\';
    char s[] = "\101\102\103-\x44\105";
    printf("%c %d %d %d %c %d %c %c\n", a, nul, nl, del, h, tab, q, bs);
    printf("%s %d\n", s, (int)sizeof(s));
    printf("%d %d %d\n", '\7', '\07', '\007');
    printf("[\101\40\x42]\n");
    return 0;
}
