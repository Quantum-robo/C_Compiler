// Edge: initializer list forms (3D, partial rows, array of strings, runtime values)
int main() {
    int k = 5;
    int cube[2][2][2] = {{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
    int part[2][3] = {{1}, {4, 5}};
    char names[2][8] = {"ab", "cd"};
    int dyn[3] = {k, k + 1, k * 2};
    return cube[1][1][1] + part[1][1] + dyn[2];
}
