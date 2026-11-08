// Feature: arrays of int and char (initializers, strings, passing to functions, char arithmetic)
int sum(int a[], int n) {
    int s = 0;
    int i;
    for (i = 0; i < n; i++) s += a[i];
    return s;
}

void fill(char buf[], int n, char c) {
    int i;
    for (i = 0; i < n; i++) buf[i] = c + i;
    buf[n] = 0;
}

int main() {
    int nums[5] = {10, 20, 30, 40, 50};
    int squares[6];
    int i;
    for (i = 0; i < 6; i++) squares[i] = i * i;
    printf("sum nums=%d squares=%d\n", sum(nums, 5), sum(squares, 6));

    nums[2] += 5;
    nums[4]++;
    printf("%d %d %d\n", nums[2], nums[4], nums[i - 6]);

    int partial[6] = {1, 2};
    for (i = 0; i < 6; i++) printf("%d ", partial[i]);
    printf("\n");

    char word[8] = "hello";
    printf("%s has %d chars\n", word, strlen(word));
    word[0] = 'J';
    word[4] = 0;
    printf("%s\n", word);

    char letters[10];
    fill(letters, 5, 'a');
    printf("%s\n", letters);

    char name[] = "array";
    printf("%s %d\n", name, strlen(name));
    int len = 0;
    while (name[len] != 0) len++;
    printf("len=%d first=%c last=%c\n", len, name[0], name[len - 1]);

    int auto_sized[] = {7, 8, 9};
    printf("%d\n", auto_sized[0] + auto_sized[1] + auto_sized[2]);

    int rev[5];
    for (i = 0; i < 5; i++) rev[4 - i] = nums[i];
    for (i = 0; i < 5; i++) printf("%d ", rev[i]);
    printf("\n");
    return 0;
}
