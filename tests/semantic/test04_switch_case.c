int main() {
    int choice = 2;
    int res = 0;

    switch (choice) {
        case 1:
            res = 100;
            break;
        case 2:
            res = 200;
            break;
        default:
            res = -1;
            break;
    }
    return res;
}
