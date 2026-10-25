struct Student {
    int id;
    float gpa;
};

int main() {
    struct Student s1;
    s1.id = 101;
    s1.gpa = 9.2;
    return s1.id;
}
