enum Color {
    RED = 1,
    GREEN = 2,
    BLUE = 3
};

union Data {
    int intVal;
    float floatVal;
    char charVal;
};

int main() {
    enum Color c = GREEN;
    union Data d;
    d.intVal = 42;
    return d.intVal;
}
