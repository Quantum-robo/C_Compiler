%{
#include <stdio.h>
#include <stdlib.h>

%}

%option noyywrap
%option yylineno

%%

/* 1. Comments */

/* 2. String literals */

/* 3. Character literals */

/* 4. Keywords */

/* 5. Identifiers */

/* 6. Multi-character operators */

/* 7. Single-character operators */

/* 8. Numbers */

/* 9. Separators */

/* 10. Whitespace */

/* 11. Invalid */

%%

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }
        yyin = file;
    }
    yylex();
    return 0;
}