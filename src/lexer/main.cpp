#include <stdio.h>
#include <vector>
#include <string>

extern int yylex();
extern FILE* yyin;

int has_error = 0;

struct LexedToken {
    std::string token;
    std::string token_type;
};

extern std::vector<LexedToken> lexed_tokens;

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