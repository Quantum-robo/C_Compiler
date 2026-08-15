%{
#include <stdio.h>
#include "Token.hpp"

void printToken(TokenType type)
{
    printf("%-20s | %s\n",
           yytext,
           tokenTypeToString(type));
}

int colno = 1;
int current_col = 1;

#define YY_USER_ACTION \
    current_col = colno; \
    for (int i = 0; i < yyleng; i++) { \
        if (yytext[i] == '\n') \
            colno = 1; \
        else \
            colno++; \
    }
%}

%option noyywrap
%option yylineno

%%

    /* Comments */
"//".*                                  {   /* ignore */ }
"/*"([^*]|\*+[^*/])*\*+"/"             {    /* ignore */ }


    /* String literals */
\"([^\"\\]|\\.)*\"    { printToken(TokenType::StringLiteral); }


    /* Character literals */
\'([^\'\\]|\\.)*\'  { printToken(TokenType::CharacterLiteral); }


    /* Keywords */
"auto"|"break"|"case"|"char"|"const"  {printToken(TokenType::Keyword);}
"continue"|"default"|"do"|"double"  {printToken(TokenType::Keyword);}
"else"|"enum"|"extern"|"float"|"for"  {printToken(TokenType::Keyword);}
"goto"|"if"|"inline"|"int"|"long"|"register"  {printToken(TokenType::Keyword);}
"restrict"|"return"|"short"|"signed"|"sizeof"  {printToken(TokenType::Keyword);}
"static"|"struct"|"switch"|"typedef"|"union" {printToken(TokenType::Keyword);}
"unsigned"|"void"|"volatile"|"while"  {printToken(TokenType::Keyword);}


    /* Identifiers */
[a-zA-Z_][a-zA-Z0-9_]* {printToken(TokenType::Identifier);}


    /* Multi-character operators */
"=="|"!="|"<="|">="|"&&"|"||"|"++"|"--"|"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<"|">>"|"->"|"<<="|">>=" {printToken(TokenType::Operator);}

    /* Single-character operators */
[+\-*/%=<>!&|^~?:.] {printToken(TokenType::Operator);}

    /* Hexadecimal literals */
0[xX][0-9a-fA-F]+ {printToken(TokenType::HexLiteral);}

    /* Floating-point literals */
[0-9]*\.[0-9]+([eE][+-]?[0-9]+)?[fFlL]? {printToken(TokenType::FloatLiteral);}

[0-9]+\.([eE][+-]?[0-9]+)?[fFlL]? {printToken(TokenType::FloatLiteral);}

[0-9]+[eE][+-]?[0-9]+[fFlL]? {printToken(TokenType::FloatLiteral);}

    /* Integer literals */
[0-9]+ {printToken(TokenType::IntegerLiteral);}

    /* Separators */
[;,\{\}\(\)\[\]] {printToken(TokenType::Separator);}

    /* Whitespace */
[ \t\n\r]+  {/* ignore */}

    /* Lexical errors */
. {fprintf(stderr,"LEXICAL ERROR: '%s' at line %d, Col: %d\n", yytext, yylineno, current_col);}
%%
int main(int argc, char **argv)
{
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");

        if (!file) {
            perror("Error opening file");
            return 1;
        }

        yyin = file;
    }

    printf("%-20s | %s\n", "Lexeme", "Token");
    printf("---------------------|----------------------\n");

    yylex();

    return 0;
}