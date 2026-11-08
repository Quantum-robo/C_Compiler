// EXPECT: invalid operands to binary '&' (have 'float' and 'int')
int main() {
    float f = 2.0f;
    int r = f & 1;
    return r;
}
