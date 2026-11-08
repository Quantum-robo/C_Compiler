// EXPECT: switch quantity is not an integer
int main() {
    float f = 1.0;
    switch (f) {
        case 1: break;
    }
    return 0;
}
