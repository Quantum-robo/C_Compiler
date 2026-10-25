int main() {
    int i = 0;
top:
    i++;
    if (i < 3) goto top;
    { goto done; }
    i = 99;
done:
    return i;
}
