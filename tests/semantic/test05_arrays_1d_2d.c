int main() {
    int nums[5];
    char name[10];
    int matrix[3][3];

    nums[0] = 42;
    nums[1] = nums[0] + 8;
    name[0] = 'P';
    name[1] = '\0';

    matrix[0][0] = 1;
    matrix[1][2] = matrix[0][0] + 10;

    return matrix[1][2];
}
