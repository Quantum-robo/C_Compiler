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

int switchTest(int n) {
    switch (n) {
        case 1:
            return 1;
        case 2:
            return 2;
        default:
            break;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 5) continue;
        if (i == 8) break;
    }

    int i = 0;
    while (i < 10) {
        i++;
    }

    do {
        i--;
    } while (i > 0);

    goto end;
end:
    return sizeof(n);
}

inline int add(int a, int b) { return a + b; }
 
/* --- Keyword-adjacent tricks --- */
class classification {
    int structure;
    int newton;
};
 
struct structure2 {
    int class_id;
    int this_field;
};