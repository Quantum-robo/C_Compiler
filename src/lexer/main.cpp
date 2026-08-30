#include <stdio.h>

extern int yylex();
extern FILE* yyin;

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }
        yyin = file;
    }

    printf("%-20s | %s\n", "Token", "Token_Type");
    printf("---------------------|----------------------\n");

    // Loop through all tokens until EOF (which returns 0)
    // The printing is handled by processToken inside Lexer.l!
    while (yylex() != 0) {
        // Keep consuming tokens
    }

    return 0;
}

