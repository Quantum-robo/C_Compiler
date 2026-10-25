int main() {
    int val = 25;
    int *ptr = &val;
    int &ref = val;

    int deref_val = *ptr;
    ref = 50;

    return deref_val;
}
