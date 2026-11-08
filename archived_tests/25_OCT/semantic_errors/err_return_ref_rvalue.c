// EXPECT: cannot return an rvalue from a function returning a non-const reference
int &f() {
    return 5;
}
int main() { return 0; }
