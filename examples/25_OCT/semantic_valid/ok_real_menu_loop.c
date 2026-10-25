int main() {
    int choice = 0;
    int total = 0;
    while (1) {
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                total += 10;
                break;
            case 2:
                total -= 5;
                break;
            case 0:
                return total;
            default:
                printf("bad\n");
                continue;
        }
        if (total > 100) break;
    }
    return total;
}
