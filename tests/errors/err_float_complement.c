// EXPECT: wrong type argument to bit-complement (type 'double')
int main() {
    double d = 1.5;
    int r = ~d;
    return r;
}
