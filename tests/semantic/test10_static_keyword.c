static int global_counter = 0;

int increment() {
    static int local_count = 0;
    local_count = local_count + 1;
    global_counter = global_counter + 1;
    return local_count;
}

int main() {
    int a = increment();
    int b = increment();
    return a + b;
}
