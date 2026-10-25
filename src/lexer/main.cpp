#include <stdio.h>
#include "Token.hpp"
#include "parser.tab.h"
YYSTYPE yylval;
extern int yylex();
extern FILE* yyin;

int has_error = 0;

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

    while (yylex() != 0) {
        // Keep consuming tokens
    }

    for (const auto& token : lexed_tokens) {
        printf("%-20s | %s\n",
               token.token.c_str(),
               token.token_type.c_str());
    }

    return has_error ? 1 : 0;
}