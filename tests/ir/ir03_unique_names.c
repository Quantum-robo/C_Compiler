// IR: every local has a name that is unique inside its function (blocks, loops, shadowed globals, statics)
int x = 1;

int counter() {
    static int n = 0;
    n++;
    return n;
}

int other() {
    static int n = 10;
    return n++;
}

int main() {
    int total = x;
    { int x = 5; total += x; }
    { int x = 6; total += x; { int x = 7; total += x; } }
    for (int i = 0; i < 2; i++) { total += i; }
    for (int i = 0; i < 3; i++) { total += i; }
    int shadow = 0;
    { int shadow = 2; total += shadow; }
    return total + counter() + other() + x;
}
