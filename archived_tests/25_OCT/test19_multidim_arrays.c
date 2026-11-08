// Feature: Multi-dimensional arrays (2D init, 3D, nested loop traversal)
int main() {
    int m[2][3] = {{1, 2, 3}, {4, 5, 6}};
    int t[2][2][2];
    int sum = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            sum = sum + m[i][j];
        }
    }
    t[0][1][1] = 9;
    t[1][0][0] = t[0][1][1] + m[1][2];
    m[1][1] = m[0][0] * 10;
    return sum;
}
