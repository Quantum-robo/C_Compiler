// EXPECT: argument 1 of 'take'
void take(int *p) { return; }
int main() {
    take(5);
    return 0;
}
