// Feature: Reference variables are aliases (reads and writes go to the referred object)
int main() {
    int val = 25;
    int &ref = val;
    int *p = &ref;
    int copy = ref;
    ref = 50;
    ref += 2;
    ref++;
    --ref;
    int arr[3];
    int &e = arr[1];
    e = 7;
    const int &c = 5;
    *p = 9;
    return ref + copy + e + c;
}
