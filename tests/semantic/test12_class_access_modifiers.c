class BankAccount {
private:
    int secretPin;
protected:
    float balance;
public:
    int accountNumber;

    void deposit(float amount) {
        balance = balance + amount;
    }

    float getBalance() {
        return balance;
    }
};

int main() {
    class BankAccount acc;
    acc.accountNumber = 12345;
    return acc.accountNumber;
}
