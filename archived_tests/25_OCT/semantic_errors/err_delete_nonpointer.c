// EXPECT: type argument of 'delete' is not a pointer
int main() {
    int a = 1;
    delete a;
    return 0;
}
