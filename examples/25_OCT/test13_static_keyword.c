// Feature: static keyword (static global, static local, static function)
static int calls = 0;

static int helper(int v) {
    return v * 2;
}

int counter() {
    static int count = 0;
    count = count + 1;
    calls++;
    return count;
}

int main() {
    counter();
    counter();
    int c = counter();
    int h = helper(c);
    return h + calls;
}
