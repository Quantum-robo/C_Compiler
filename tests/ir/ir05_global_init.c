// IR: global and static initialisers (arrays, strings, structs, const sizes) run from _global_init
struct Pt { int x; int y; };
int g = 3;
int table[4] = {1, 2, 3, 4};
char greeting[8] = "hi";
struct Pt origin = {5, 6};
const int N = 3;
int sized[N];

int main() {
    static int calls = 0;
    int buf[N];
    calls++;
    sized[1] = g + table[2] + origin.y + greeting[1];
    buf[0] = sized[1];
    return buf[0] + calls;
}
