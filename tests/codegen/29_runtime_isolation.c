// A program may define functions and globals with the names the runtime library uses internally
// (rt_pad, rt_digits, rt_emit, rt_udiv, rt_seed, rt_line, rt_pos, rt_free_list): the runtime keeps its own labels.
int rt_seed = 5;
char rt_line[4] = "usr";
int rt_pos = 99;

int rt_pad(int a) { return a + 1; }
int rt_digits(int a, int b) { return a * b; }
int rt_emit() { return 7; }
int rt_udiv(int a, int b) { return a - b; }

struct RtBlock {
    char name[8];
    int free_list;
};

int main() {
    printf("%d %d %d %d\n", rt_pad(1), rt_digits(6, 7), rt_emit(), rt_udiv(10, 3));
    printf("%d %s %d\n", rt_seed, rt_line, rt_pos);
    struct RtBlock b;
    strcpy(b.name, "mine");
    b.free_list = 3;
    printf("%s %d\n", b.name, b.free_list);
    int *p = malloc(8);
    p[0] = 11;
    p[1] = 22;
    printf("%d\n", p[0] + p[1]);
    free(p);
    printf("%5d|%-5d|%x\n", 42, 42, 255);
    return 0;
}
