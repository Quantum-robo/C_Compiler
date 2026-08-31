class MyClass {
private:
    int value;
public:
    void setValue(int v) {
        this->value = v;
    }
};

struct Point {
    int x;
    int y;
};

int add(int a, int b) {
    return a + b;
}

int main() {
    int a = 10;
    int b = 5;

    int arithmetic = (a + b) * 2 - 3;
    int comparison = a <= 10 && b != 0;
    int bitwise = (a & b) | (a ^ b);
    int shifted = a << 2;

    arithmetic += 5;
    arithmetic -= 2;
    arithmetic *= 3;

    int values[10];
    values[0] = add(a, b);

    values[1]++;
    ++values[2];

    int result = values[0] > 10 ? values[0] : 10;

    return result;
}
