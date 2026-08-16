class Student {
    int age;
    void setAge(int value) {
        this->age = value;
    }
};

int main() {
    Student *s = new Student;
    delete s;
    return 0;
}