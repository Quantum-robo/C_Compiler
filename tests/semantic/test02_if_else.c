int main() {
    int score = 85;
    int grade = 0;

    if (score >= 90) {
        grade = 10;
    } else if (score >= 80) {
        grade = 9;
    } else {
        grade = 8;
    }
    return grade;
}
