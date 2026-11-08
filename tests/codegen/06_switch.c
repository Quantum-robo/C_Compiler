// Feature: switch (fall-through, default in the middle, char and int cases, nested switch)
int main() {
    int i;
    for (i = 0; i < 6; i++) {
        switch (i) {
            case 0:
                printf("zero\n");
                break;
            case 1:
            case 2:
                printf("one or two\n");
                break;
            case 3:
                printf("three, ");
            default:
                printf("default (%d)\n", i);
                break;
            case 4:
                printf("four\n");
        }
    }

    char c = 'b';
    switch (c) {
        case 'a': printf("is a\n"); break;
        case 'b': printf("is b\n"); break;
        case 'c': printf("is c\n"); break;
    }

    int x = 7;
    switch (x) {
        case 1: printf("one\n"); break;
        case 2: printf("two\n"); break;
    }
    printf("after switch\n");

    int outer = 1, inner = 2;
    switch (outer) {
        case 1:
            switch (inner) {
                case 1: printf("1-1\n"); break;
                case 2: printf("1-2\n"); break;
            }
            printf("end of case 1\n");
            break;
        case 2:
            printf("2\n");
            break;
    }
    return 0;
}
