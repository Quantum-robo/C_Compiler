// Feature: enum values (implicit, explicit, negative, hex, char) used in a switch and in expressions
enum Op { ADD, SUB = -3, MUL, HEX = 0x10, CH = 'A', NEXT };
enum Other { P = 7, Q };
int f(int x) {
    switch (x) {
        case ADD: return 1;
        case SUB: return 2;
        case Q: return 3;
    }
    int ZERO = 9;
    return ZERO + MUL + HEX + CH + NEXT;
}
int main() { enum Op o = NEXT; return f(o); }
