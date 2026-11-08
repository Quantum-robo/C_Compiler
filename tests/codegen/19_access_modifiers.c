// Feature: public, private and protected (legal uses; illegal uses are rejected, see tests/semantic and archived_tests)
class Account {
private:
    int pin;
    int balance;
protected:
    int kind;
public:
    int id;

    void open(int i, int p, int k) {
        id = i;
        pin = p;
        kind = k;
        balance = 0;
    }
    void deposit(int amount) { balance += amount; }
    int withdraw(int amount) {
        if (amount > balance) return 0;
        balance -= amount;
        return 1;
    }
    int check(int p) { return p == pin; }
    int get_balance() { return balance; }
    int get_kind() { return kind; }
};

int main() {
    class Account a;
    a.open(7, 1234, 2);
    a.deposit(100);
    printf("%d\n", a.withdraw(30));
    printf("%d\n", a.withdraw(500));
    printf("%d %d %d\n", a.get_balance(), a.check(1234), a.check(1));
    printf("id=%d kind=%d\n", a.id, a.get_kind());
    a.id = 8;
    printf("%d\n", a.id);
    return 0;
}
