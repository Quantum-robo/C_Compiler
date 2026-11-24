// EXPECT: invalid application of 'sizeof' to incomplete type 'Missing'
int main() {
    return sizeof(struct Missing);
}
