// Feature: enum and union
enum Color {
    RED,
    GREEN = 5,
    BLUE
};

union Value {
    int i;
    float f;
    char c;
};

int main() {
    enum Color col = BLUE;
    int code = 0;
    if (col == GREEN) {
        code = 1;
    } else {
        code = 2;
    }
    union Value v;
    v.i = 65;
    v.c = 'A';
    v.f = 3.14;
    int k = v.i + RED;
    return code + k;
}
