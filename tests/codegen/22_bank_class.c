// Integration: classes with private data, an array of objects, objects on the heap and in structs
class Account {
private:
    int id;
    int balance;
    int history[4];
    int n;
public:
    void open(int account_id, int initial) {
        id = account_id;
        balance = initial;
        n = 0;
        log(initial);
    }
    void log(int amount) {
        if (n < 4) {
            history[n] = amount;
            n++;
        }
    }
    int deposit(int amount) {
        if (amount <= 0) return 0;
        balance += amount;
        log(amount);
        return 1;
    }
    int withdraw(int amount) {
        if (amount <= 0 || amount > balance) return 0;
        balance -= amount;
        log(-amount);
        return 1;
    }
    int transfer(class Account *to, int amount) {
        if (!withdraw(amount)) return 0;
        to->deposit(amount);
        return 1;
    }
    int get_id() { return id; }
    int get_balance() { return balance; }
    int logged() { return n; }
    int logged_at(int i) { return history[i]; }
};

class Bank {
public:
    class Account accounts[3];
    int count;

    void setup() {
        int i;
        count = 3;
        for (i = 0; i < count; i++) accounts[i].open(100 + i, 50 * (i + 1));
    }
    int total() {
        int i, t = 0;
        for (i = 0; i < count; i++) t += accounts[i].get_balance();
        return t;
    }
};

int main() {
    class Bank bank;
    bank.setup();
    printf("total=%d\n", bank.total());
    printf("%d\n", bank.accounts[0].transfer(&bank.accounts[2], 30));
    printf("%d\n", bank.accounts[1].transfer(&bank.accounts[0], 500));
    printf("%d\n", bank.accounts[2].deposit(-5));
    int i;
    for (i = 0; i < 3; i++) {
        class Account *a = &bank.accounts[i];
        printf("account %d balance %d log:", a->get_id(), a->get_balance());
        int j;
        for (j = 0; j < a->logged(); j++) printf(" %d", a->logged_at(j));
        printf("\n");
    }
    printf("total=%d\n", bank.total());

    class Account *heap = new class Account;
    heap->open(999, 10);
    heap->deposit(5);
    printf("%d %d\n", heap->get_id(), heap->get_balance());
    delete heap;
    return 0;
}
