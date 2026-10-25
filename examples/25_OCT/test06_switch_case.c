// Feature: switch cases (default, fallthrough, switch inside loop)
int main() {
    int day = 3;
    int kind = 0;
    switch (day) {
        case 1:
            kind = 10;
            break;
        case 2:
        case 3:
            kind = 20;
            break;
        case 4:
            kind = 40;
        default:
            kind = kind + 1;
            break;
    }
    int score = 0;
    for (int i = 0; i < 3; i++) {
        switch (i) {
            case 0:
                score += 5;
                break;
            default:
                score += 1;
        }
    }
    return kind + score;
}
