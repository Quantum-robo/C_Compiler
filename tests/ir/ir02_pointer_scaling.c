// IR: pointer arithmetic moves by whole elements; ++ and += scale too; p - q counts elements
struct Rec { int id; char tag; };

int main() {
    int nums[4] = {1, 2, 3, 4};
    char text[6] = "hello";
    struct Rec recs[4];
    int grid[2][3] = {{1, 2, 3}, {4, 5, 6}};
    int *ip = nums;
    char *cp = text;
    struct Rec *rp = recs;
    ip = ip + 2;
    ip--;
    ip += 1;
    cp = cp + 3;
    cp--;
    rp = rp + 2;
    rp++;
    long d1 = ip - nums;
    long d2 = rp - recs;
    int *row = grid[1];
    int v = *(ip + 1) + cp[1] + rp[1].id + grid[1][2];
    return d1 + d2 + v + *(row + 1);
}
