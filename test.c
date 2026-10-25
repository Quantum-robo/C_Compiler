struct Node {
    int val;
};

int sideEffect(int x) {
    return x + 1;
}

int main() {
    int cube[2][3][4];
    struct Node list[5];
    int a = 1, b = 2, c = 3;
    int res = 0;

    // 1. 3D array index calculation: ((a * 3) + b) * 4 + c
    cube[a][b][c] = 42;

    // 2. Array of structs
    list[a + 1].val = cube[a][b][c];

    // 3. Dangling else + Short-circuit with function call
    if (a == 1)
        if (b == 0 && sideEffect(a) > 0)
            res = 100;
        else
            res = list[2].val;

    return res;
}