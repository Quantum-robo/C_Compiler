enum Color { RED, GREEN = 5, BLUE };
const int LIMIT = 10;
int main() {
    enum Color c = BLUE;
    int total = 0;
    switch (c) {
        case RED: total = 1; break;
        case GREEN: total = 2; break;
        default: total = LIMIT + RED; break;
    }
    int arr[LIMIT];
    return total + arr[0];
}
