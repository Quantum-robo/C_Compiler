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

    /* COMMENTS */
    //  "//".* {/* Ignore single-line comment */}

    //  "/*"([^*]|\*+[^*/])*\*+"/" {/* Ignore multi-line comment */}

"//".*                                  {   /* ignore */ }
"/*"([^*]|\*+[^*/])*\*+"/"             {    /* ignore */ }

"/*"([^*]|\*+[^*/])* {
    fprintf(stderr,
        "LEXICAL ERROR:unterminated block comment till line %d\n",
        yylineno-1
    );

        //currentToken = {TokenType::Unknown, yytext};return 1;
}
    /*  STRING LITERALS  */
\"([^\"\\\n]|\\.)*\" {printToken(TokenType::StringLiteral);}

\"([^\"\\\n]|\\.)*\n {
    fprintf(stderr,
        "LEXICAL ERROR:unterminated string literal %s at line %d\n",
        yytext,
        yylineno-1
    );

     // currentToken = {TokenType::Unknown, yytext};return 1;
}

    /*  CHARACTER LITERALS  */
\'([^\'\\\n]|\\.|\\x[0-9a-fA-F]+)\' {printToken(TokenType::CharacterLiteral); }
    /*\'([^\'\\\n]|\\.)\' {printToken(TokenType::CharacterLiteral);}*/
\'([^\'\\\n]|\\.){2,}\' {
    fprintf(stderr,
        "LEXICAL ERROR:invalid character literal %s at line %d\n",
        yytext,
        yylineno-1);
        }

\'([^\'\\\n]|\\.)*\n {
    fprintf(stderr,
        "LEXICAL ERROR: unterminated character literal %.*s at line %d\n",
        yyleng,
        yytext,
        yylineno-1
    );
}


    /*  KEYWORDS  */
"auto"      { printToken(TokenType::KwAuto); }
"break"     { printToken(TokenType::KwBreak); }
"case"      { printToken(TokenType::KwCase); }
"char"      { printToken(TokenType::KwChar); }
"class"     { printToken(TokenType::KwClass); }
"const"     { printToken(TokenType::KwConst); }
"continue"  { printToken(TokenType::KwContinue); }
"default"   { printToken(TokenType::KwDefault); }
"delete"    { printToken(TokenType::KwDelete); }
"do"        { printToken(TokenType::KwDo); }
"double"    { printToken(TokenType::KwDouble); }
"else"      { printToken(TokenType::KwElse); }
"enum"      { printToken(TokenType::KwEnum); }
"extern"    { printToken(TokenType::KwExtern); }
"float"     { printToken(TokenType::KwFloat); }
"for"       { printToken(TokenType::KwFor); }
"goto"      { printToken(TokenType::KwGoto); }
"if"        { printToken(TokenType::KwIf); }
"inline"    { printToken(TokenType::KwInline); }
"int"       { printToken(TokenType::KwInt); }
"long"      { printToken(TokenType::KwLong); }
"new"       { printToken(TokenType::KwNew); }
"public"    { printToken(TokenType::KwPublic); }
"private"   { printToken(TokenType::KwPrivate); }
"protected" { printToken(TokenType::KwProtected); }
"register"  { printToken(TokenType::KwRegister); }
"restrict"  { printToken(TokenType::KwRestrict); }
"return"    { printToken(TokenType::KwReturn); }
"short"     { printToken(TokenType::KwShort); }
"signed"    { printToken(TokenType::KwSigned); }
"sizeof"    { printToken(TokenType::KwSizeof); }
"static"    { printToken(TokenType::KwStatic); }
"struct"    { printToken(TokenType::KwStruct); }
"switch"    { printToken(TokenType::KwSwitch); }
"this"      { printToken(TokenType::KwThis); }
"typedef"   { printToken(TokenType::KwTypedef); }
"union"     { printToken(TokenType::KwUnion); }
"unsigned"  { printToken(TokenType::KwUnsigned); }
"void"      { printToken(TokenType::KwVoid); }
"volatile"  { printToken(TokenType::KwVolatile); }
"while"     { printToken(TokenType::KwWhile); }


    /*  IDENTIFIERS  */
[a-zA-Z_][a-zA-Z0-9_]* {printToken(TokenType::Identifier);}


    /*  MULTI-CHARACTER OPERATORS  */

"<<"= {printToken(TokenType::ShiftLeftAssign);}
">>"= {printToken(TokenType::ShiftRightAssign);}
"+=" {printToken(TokenType::PlusAssign);}
"-=" {printToken(TokenType::MinusAssign);}
"*=" {printToken(TokenType::MultiplyAssign);}
"/=" {printToken(TokenType::DivideAssign);}
"%=" {printToken(TokenType::ModuloAssign);}
"&=" {printToken(TokenType::BitwiseAndAssign);}
"|=" {printToken(TokenType::BitwiseOrAssign);}
"^=" {printToken(TokenType::BitwiseXorAssign);}
"==" {printToken(TokenType::Equal);}
"!=" {printToken(TokenType::NotEqual);}
"<=" {printToken(TokenType::LessEqual);}
">=" {printToken(TokenType::GreaterEqual);}
"&&" {printToken(TokenType::LogicalAnd);}
"||" {printToken(TokenType::LogicalOr);}
"++" {printToken(TokenType::Increment);}
"--" {printToken(TokenType::Decrement);}
"<<" {printToken(TokenType::ShiftLeft);}
">>" {printToken(TokenType::ShiftRight);}
"->" {printToken(TokenType::Arrow);}


    /*  SINGLE-CHAR OPERATORS  */

"+"  { printToken(TokenType::Plus); }
"-"  { printToken(TokenType::Minus); }
"*"  { printToken(TokenType::Star); }
"/"  { printToken(TokenType::Slash); }
"%"  { printToken(TokenType::Percent); }
"="  { printToken(TokenType::Assign); }

"<"  { printToken(TokenType::Less); }
">"  { printToken(TokenType::Greater); }

"!"  { printToken(TokenType::LogicalNot); }
"&"  { printToken(TokenType::BitwiseAnd); }
"|"  { printToken(TokenType::BitwiseOr); }
"^"  { printToken(TokenType::BitwiseXor); }
"~"  { printToken(TokenType::BitwiseNot); }


    /* Hexadecimal */
0[xX][0-9a-fA-F]+                         {printToken(TokenType::HexLiteral);}

    /* Floating point */
[0-9]*\.[0-9]+([eE][+-]?[0-9]+)?[fFlL]?   {printToken(TokenType::FloatLiteral);}
[0-9]+\.([eE][+-]?[0-9]+)?[fFlL]?         {printToken(TokenType::FloatLiteral);}
[0-9]+[eE][+-]?[0-9]+[fFlL]?              {printToken(TokenType::FloatLiteral);}

    /* Integer */
[0-9]+                                    {printToken(TokenType::IntegerLiteral);}


    /*  PUNCTUATORS  */

";" { printToken(TokenType::Semicolon); }
"," { printToken(TokenType::Comma); }
"." { printToken(TokenType::Dot); }
":" { printToken(TokenType::Colon); }
"?" { printToken(TokenType::Question); }

"(" { printToken(TokenType::LeftParen); }
")" { printToken(TokenType::RightParen); }

"[" { printToken(TokenType::LeftBracket); }
"]" { printToken(TokenType::RightBracket); }

"{" { printToken(TokenType::LeftBrace); }
"}" { printToken(TokenType::RightBrace); }


    /*  WHITESPACE  */
[ \t\n\r]+              {   /* Ignore whitespace */ }


    /*  LEXICAL ERRORS  */

. {
    fprintf(
        stderr,
        "LEXICAL ERROR: '%s' at line %d\n",
        yytext,
        yylineno
    );
}

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