// EXPECT: 'secret' is private within this context
class Vault {
private:
    int secret;
public:
    int open;
};
int main() {
    class Vault v;
    v.secret = 1;
    return 0;
}
