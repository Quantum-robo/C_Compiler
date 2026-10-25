// Feature: References to structs, objects, pointers and arrays; method with reference parameter
struct P {
    int x;
    int y;
};

class Acc {
public:
    int n;
    void bump(int &k) {
        k = k + n;
    }
    int get() {
        return n;
    }
};

void setx(struct P &p) {
    p.x = 3;
}

int sum(int (&a)[3]) {
    return a[0] + a[2];
}

int main() {
    struct P q;
    struct P &rq = q;
    rq.x = 1;
    rq.y = 2;
    setx(q);
    class Acc acc;
    class Acc &ra = acc;
    int v = 4;
    acc.n = 2;
    acc.bump(v);
    ra.bump(v);
    int t = ra.get();
    int *p = &v;
    int *&rp = p;
    *rp = 8;
    int nums[3];
    int (&rn)[3] = nums;
    rn[1] = 5;
    int s = sum(nums);
    return q.x + v + t + s;
}
