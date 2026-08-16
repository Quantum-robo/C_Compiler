%{
#include "Lexer.hpp"

#define YY_DECL int Lexer::yylex()

int colno = 1;
int current_col = 1;
int token_start_line = 1;
int token_start_col = 1;

#define YY_USER_ACTION \
    token_start_line = yylineno; \
    token_start_col = colno; \
    current_col = colno; \
    for (int i = 0; i < yyleng; i++) { \
        if (yytext[i] == '\n') { \
            token_start_line--; \
            colno = 1; \
        } else { \
            colno++; \
        } \
    }
%}

%option c++
%option noyywrap
%option yylineno
%option yyclass="Lexer"

%%

    /* Comments */
"//".*                                  {   /* ignore */ }
"/*"([^*]|\*+[^*/])*\*+"/"             {    /* ignore */ }

"/*"([^*]|\*+[^*/])* {
    addError(
        "unterminated block comment",
        token_start_line,
        token_start_col
    );

    currentToken = {TokenType::Unknown, yytext};
    return 1;
}

    /* String literals */
\"([^\"\\]|\\.)*\"    {
    currentToken = {TokenType::StringLiteral, yytext};
    return 1;
}

\"([^\"\\]|\\.)*\n {
    addError(
        "unterminated string literal",
        token_start_line,
        token_start_col
    );

    currentToken = {TokenType::Unknown, yytext};
    return 1;
}

    /* Character literals */
\'([^\'\\]|\\.)\' {
    currentToken = {TokenType::CharacterLiteral, yytext};
    return 1;
}

\'([^\'\\]|\\.){2,}\' {
    addError(
        "invalid character literal",
        token_start_line,
        token_start_col
    );

    currentToken = {TokenType::Unknown, yytext};
    return 1;
}

\'([^\'\\]|\\.)*\n {
    addError(
        "unterminated character literal",
        token_start_line,
        token_start_col
    );

    currentToken = {TokenType::Unknown, yytext};
    return 1;
}

    /* Keywords */
"auto"|"break"|"case"|"char"|"const" {
    currentToken = {TokenType::Keyword, yytext};
    return 1;
}

"continue"|"default"|"do"|"double" {
    currentToken = {TokenType::Keyword, yytext};
    return 1;
}

"else"|"enum"|"extern"|"float"|"for" {
    currentToken = {TokenType::Keyword, yytext};
    return 1;
}

"goto"|"if"|"inline"|"int"|"long"|"register" {
    currentToken = {TokenType::Keyword, yytext};
    return 1;
}

"restrict"|"return"|"short"|"signed"|"sizeof" {
    currentToken = {TokenType::Keyword, yytext};
    return 1;
}

"static"|"struct"|"switch"|"typedef"|"union" {
    currentToken = {TokenType::Keyword, yytext};
    return 1;
}

"unsigned"|"void"|"volatile"|"while" {
    currentToken = {TokenType::Keyword, yytext};
    return 1;
}

"class"|"new"|"delete"|"this" {
    currentToken = {TokenType::Keyword, yytext};
    return 1;
}

    /* Identifiers */
[a-zA-Z_][a-zA-Z0-9_]* {
    currentToken = {TokenType::Identifier, yytext};
    return 1;
}


    /* Multi-character operators */
"=="|"!="|"<="|">="|"&&"|"||"|"++"|"--"|"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<"|">>"|"->"|"<<="|">>=" {
    currentToken = {TokenType::Operator, yytext};
    return 1;
}

    /* Single-character operators */
[+\-*/%=<>!&|^~?:.] {
    currentToken = {TokenType::Operator, yytext};
    return 1;
}

    /* Hexadecimal literals */
0[xX][0-9a-fA-F]+ {
    currentToken = {TokenType::HexLiteral, yytext};
    return 1;
}

    /* Floating-point literals */
[0-9]*\.[0-9]+([eE][+-]?[0-9]+)?[fFlL]? {
    currentToken = {TokenType::FloatLiteral, yytext};
    return 1;
}

[0-9]+\.([eE][+-]?[0-9]+)?[fFlL]? {
    currentToken = {TokenType::FloatLiteral, yytext};
    return 1;
}

[0-9]+[eE][+-]?[0-9]+[fFlL]? {
    currentToken = {TokenType::FloatLiteral, yytext};
    return 1;
}

    /* Integer literals */
[0-9]+ {
    currentToken = {TokenType::IntegerLiteral, yytext};
    return 1;
}

    /* Separators */
[;,\{\}\(\)\[\]] {
    currentToken = {TokenType::Separator, yytext};
    return 1;
}

    /* Whitespace */
[ \t\n\r]+  {/* ignore */}

    /* Lexical errors */
. {
    addError(
        std::string("invalid character '") + yytext + "'",
        yylineno,
        current_col
    );

    currentToken = {TokenType::Unknown, yytext};
    return 1;
}
%%
Lexer::Lexer(std::istream* input)
    : yyFlexLexer(input)
{
}

void Lexer::addError(const std::string& message, int line, int column)
{
    lexicalErrors.push_back({message, line, column});
}

const std::vector<LexicalError>& Lexer::errors() const
{
    return lexicalErrors;
}

Token Lexer::nextToken()
{
    if (yylex() == 0) {
        currentToken = {TokenType::EndOfFile, ""};
    }

    return currentToken;
}