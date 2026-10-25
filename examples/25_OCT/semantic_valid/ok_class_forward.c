class Shape {
private:
    int secret;
public:
    int area() { return width * height + helper(); }
    int helper() { return secret + count(); }
    int count() { return 1; }
    int same(class Shape &o) { return o.secret; }
    int width;
    int height;
};
int main() {
    class Shape s;
    s.width = 2;
    s.height = 3;
    return s.area();
}
