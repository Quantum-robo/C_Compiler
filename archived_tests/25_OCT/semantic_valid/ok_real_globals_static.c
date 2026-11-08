static int counter = 0;
const int MAX = 10;
int sieve[100];
int next() {
    counter++;
    return counter;
}
bool isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}
int main() {
    int total = 0;
    do {
        total += next();
    } while (counter < MAX);
    for (int i = 0; i < 100; i++) sieve[i] = isPrime(i);
    int a = 5, b = 6, c;
    c = (a > b) ? a : b;
    long long big = 100000;
    unsigned int u = 3;
    return total + sieve[7] + c + big + u;
}
