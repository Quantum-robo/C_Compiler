int main() {
    // Error 1: Invalid character
    int z = 10 @ 20;

    // Error 2: Invalid character literal
    char c = 'ab';

    // Error 3: Missing expression after equals
    int x = ;

    // Error 4: Missing parentheses around the condition
    if x > 0 {
        x = 1;
    }

    // Error 5: Missing semicolon
    int y = 5

    // Error 6: Unterminated string literal - must be last
    char *s = "unterminated;