// printf conversions, flags, widths and return value
int main() {
    int n = 42, neg = -42;
    printf("[%d] [%5d] [%-5d] [%05d] [%+d] [% d]\n", n, n, n, n, n, n);
    printf("[%d] [%5d] [%-5d] [%05d] [%+d]\n", neg, neg, neg, neg, neg);
    printf("[%x] [%X] [%o] [%u]\n", 255, 255, 8, 7);
    printf("[%x] [%u] [%x] [%08x]\n", -1, -1, 4096, 48879);
    printf("[%c] [%3c] [%-3c]\n", 'a', 'b', 'c');
    printf("[%s] [%8s] [%-8s] [%.2s] [%8.3s]\n", "text", "text", "text", "text", "text");
    printf("100%% done\n");
    printf("%d %d\n", 2147483647, -2147483647 - 1);
    int len = printf("count me\n");
    printf("%d\n", len);
    printf("[%*d] [%-*d]\n", 6, 12, 6, 12);
    printf("%ld %hd\n", 123456, 7);
    printf("[%s]\n", "");
    printf("%d%d%d\n", 1, 2, 3);
    printf("%5s|%-5s|\n", "toolongvalue", "x");
    printf("%03d:%02d\n", 7, 5);
    return 0;
}
