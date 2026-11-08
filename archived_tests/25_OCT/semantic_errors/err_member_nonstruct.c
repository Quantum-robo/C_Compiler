// EXPECT: request for member 'x' in something not a structure or union
int main() {
    int a = 1;
    a.x = 3;
    return 0;
}
