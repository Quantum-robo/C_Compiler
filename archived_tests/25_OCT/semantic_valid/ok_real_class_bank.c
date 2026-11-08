class Account {
private:
    int balance;
    int id;
public:
    int open(int start) {
        balance = start;
        return balance;
    }
    int deposit(int amt) {
        if (amt > 0) {
            balance += amt;
        }
        return balance;
    }
    int withdraw(int amt) {
        if (amt > balance) return -1;
        balance = balance - amt;
        return balance;
    }
    int getBalance() { return balance; }
};
int main() {
    class Account acc;
    class Account *p = &acc;
    acc.open(100);
    p->deposit(50);
    acc.withdraw(30);
    printf("%d\n", acc.getBalance());
    return acc.getBalance();
}
