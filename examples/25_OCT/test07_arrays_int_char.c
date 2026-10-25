// Feature: 1D arrays of int and char
int main() {
    int nums[5] = {10, 20, 30, 40, 50};
    int squares[5];
    char word[6] = "hello";
    char letters[3];
    int sum = 0;

    for (int i = 0; i < 5; i++) {
        squares[i] = i * i;
        sum = sum + nums[i];
    }
    letters[0] = 'a';
    letters[1] = 'b';
    letters[2] = '\0';
    word[0] = letters[1];
    nums[2] = nums[1] + nums[3];
    nums[sum % 5] += 1;
    return sum;
}
