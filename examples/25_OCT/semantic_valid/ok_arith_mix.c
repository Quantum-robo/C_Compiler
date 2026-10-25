int main() {
    char c = 65;
    int digit = '7' - '0';
    short s = 3;
    long l = 100;
    unsigned int u = 5;
    float f = 1;
    double d = f * 2.5;
    bool b = true;
    int x = c + s;
    l = l * x;
    f = d;
    x = (b ? 1 : 0) + (f > 2.0);
    x <<= 2;
    x |= 1;
    x = x % 3;
    c += 1;
    c++;
    b = x && c;
    b = !b;
    x = ~x;
    char *msg = "hello";
    const char *cm = "world";
    char word[10] = "abc";
    int big = 10 / 3 + 7 % 2;
    return x + digit + big;
}
