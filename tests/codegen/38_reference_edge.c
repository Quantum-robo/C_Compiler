// Reference edge cases: aliases of array elements and struct members, reference chains, const references,
// references through recursion, swapping structs, references in loops
struct P {
    int x;
    int y;
};

class Box {
public:
    int items[3];
    P corner;
    int &item(int i) { return items[i]; }
    P &get_corner() { return corner; }
};

void swap_int(int &a, int &b) {
    int t = a;
    a = b;
    b = t;
}

void swap_p(P &a, P &b) {
    P t = a;
    a = b;
    b = t;
}

void bump_all(int &a, int &b, int &c) {
    a++;
    b++;
    c++;
}

void count_down(int &n, int &steps) {
    if (n == 0) return;
    n--;
    steps++;
    count_down(n, steps);
}

int sum_ref(const int &a, const int &b) { return a + b; }

int &pick(int arr[], int i) { return arr[i]; }

int main() {
    int arr[4] = {1, 2, 3, 4};
    int &first = arr[0];
    int &last = arr[3];
    first = 100;
    last += 5;
    printf("%d %d %d\n", arr[0], arr[3], first + last);
    swap_int(arr[1], arr[2]);
    printf("%d %d\n", arr[1], arr[2]);

    int x = 1;
    int &r1 = x;
    int &r2 = r1;
    r2 = 5;
    r1++;
    printf("%d %d %d\n", x, r1, r2);

    P a, b;
    a.x = 1; a.y = 2;
    b.x = 3; b.y = 4;
    swap_p(a, b);
    printf("%d %d %d %d\n", a.x, a.y, b.x, b.y);
    int &ay = a.y;
    ay = 50;
    printf("%d\n", a.y);

    int p = 0, q = 0, r = 0;
    bump_all(p, q, r);
    bump_all(p, p, r);
    printf("%d %d %d\n", p, q, r);

    int n = 7, steps = 0;
    count_down(n, steps);
    printf("%d %d\n", n, steps);
    printf("%d %d\n", sum_ref(3, 4), sum_ref(x, x));

    pick(arr, 2) = 99;
    pick(arr, 3)++;
    printf("%d %d\n", arr[2], arr[3]);

    Box box;
    box.item(0) = 7;
    box.item(1) = box.item(0) * 2;
    box.items[2] = 1;
    P &c = box.get_corner();
    c.x = 8;
    c.y = box.item(1);
    printf("%d %d %d %d %d\n", box.items[0], box.items[1], box.items[2], box.corner.x, box.corner.y);

    int total = 0;
    int i;
    for (i = 0; i < 4; i++) {
        int &e = arr[i];
        e = e * 2;
        total += e;
    }
    printf("%d %d %d\n", total, arr[0], arr[3]);
    return 0;
}
