// Feature: public, private and protected keywords
class Account {
private:
    int pin;
    float balance;
protected:
    int accountType;
public:
    int id;

    void setPin(int p) {
        pin = p;
    }

    void deposit(float amt) {
        if (amt > 0) {
            balance = balance + amt;
        }
    }

    float getBalance() {
        return balance;
    }
};

int main() {
    class Account acc;
    acc.id = 7;
    acc.setPin(1234);
    acc.deposit(500.5);
    float b = acc.getBalance();
    return acc.id;
}
