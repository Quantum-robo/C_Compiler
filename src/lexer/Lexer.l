%{
#include <stdio.h>
#include <string.h> // For strdup
#include "Token.hpp"
#include "parser.tab.h" // Hooking up Bison's tokens
#include <map>
#include <string>
#include <cctype>
extern int has_error;
extern FILE* diag_out;
extern bool (*type_name_hook)(const char*);   // set by the parser; null in the standalone lexer

std::vector<LexedToken> lexed_tokens;

// Has a type just been written? ("struct node", "Counter", "int", ...) A name that follows it, directly or after
// '*' / '&', is the declared variable even if it is also a type name:  struct node *node;   Counter Counter;
static bool afterType = false;
static int previousToken = 0;

static void trackTypeContext(int tok)
{
    bool tagPosition = previousToken == KW_STRUCT || previousToken == KW_CLASS ||
                       previousToken == KW_UNION || previousToken == KW_ENUM;
    switch (tok) {
        case TYPE_NAME: case KW_STRUCT: case KW_CLASS: case KW_UNION: case KW_ENUM:
        case KW_VOID: case KW_CHAR: case KW_SHORT: case KW_INT: case KW_LONG: case KW_FLOAT:
        case KW_DOUBLE: case KW_SIGNED: case KW_UNSIGNED: case KW_BOOL:
            afterType = true;
            break;
        case IDENTIFIER:
            afterType = tagPosition;                  // the name of a struct: the type is complete
            break;
        case STAR: case BITWISE_AND: case KW_CONST:
            break;                                    // still inside the type
        default:
            afterType = false;
    }
    previousToken = tok;
}

int processToken(TokenType type, int bison_token)
{
    trackTypeContext(bison_token);
    lexed_tokens.push_back({
        yytext,
        tokenTypeToString(type)
    });

    // Pass string value to Bison for identifiers and literals
    yylval.str = strdup(yytext);

    return bison_token;
}

int colno = 1;
int current_col = 1;

// Object-like macros:  #define NAME replacement text   (forgotten again by #undef NAME).
// An identifier that names a macro is replaced by the text, which is then scanned again.
// Function-like macros (#define F(x) ...) are not supported; conditional-compilation lines are ignored.
static std::map<std::string, std::string> macros;
static int macroExpansions = 0;
static bool expand_macro(const char* name);     // defined after the rules, where unput() exists

void lexer_reset_macros()
{
    macros.clear();
    macroExpansions = 0;
    afterType = false;
    previousToken = 0;
}

static std::string trim(const std::string& s)
{
    size_t b = s.find_first_not_of(" \t\r");
    if (b == std::string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r");
    return s.substr(b, e - b + 1);
}

// A preprocessor line: only "#define NAME text" and "#undef NAME" have an effect.
static void directive(const std::string& line)
{
    size_t i = 1;                                        // after '#'
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;
    size_t w = i;
    while (i < line.size() && isalpha((unsigned char)line[i])) ++i;
    std::string word = line.substr(w, i - w);
    if (word != "define" && word != "undef") return;
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;
    size_t n = i;
    while (i < line.size() && (isalnum((unsigned char)line[i]) || line[i] == '_')) ++i;
    std::string name = line.substr(n, i - n);
    if (name.empty()) return;
    if (word == "undef") { macros.erase(name); return; }
    if (i < line.size() && line[i] == '(') return;       // a function-like macro: not supported
    std::string text = line.substr(i);
    size_t c = text.find("//");                          // a trailing comment is not part of the text
    if (c != std::string::npos) text = text.substr(0, c);
    c = text.find("/*");
    if (c != std::string::npos) text = text.substr(0, c);
    macros[name] = trim(text);
}


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
%option never-interactive

%%

    /* PREPROCESSOR DIRECTIVES */
"#"[^\n]*                               { directive(yytext); }

    /* COMMENTS */


"//".*                                  {   /* ignore */ }
"/*"([^*]|\*+[^*/])*\*+"/"             {    /* ignore */ }

"/*"([^*]|\*+[^*/])* {
    fprintf(diag_out,
        "LEXICAL ERROR:unterminated block comment till line %d\n",
        yylineno-1
    );

    has_error = 1;

        //currentToken = {TokenType::Unknown, yytext};return 1;
}
    /*  STRING LITERALS  */
\"([^\"\\\n]|\\.)*\" { return processToken(TokenType::StringLiteral, STRING_LITERAL); }

\"([^\"\\\n]|\\.)*\n {
    fprintf(diag_out,
        "LEXICAL ERROR: unterminated string literal %s at line %d\n",
        yytext,
        yylineno-1
    );

    has_error = 1;

     // currentToken = {TokenType::Unknown, yytext};return 1;
}

    /*  CHARACTER LITERALS  */
\'([^\'\\\n]|\\.|\\x[0-9a-fA-F]+)\' { return processToken(TokenType::CharacterLiteral, CHAR_LITERAL); }
    /*\'([^\'\\\n]|\\.)\' { return processToken(TokenType::CharacterLiteral, CHAR_LITERAL);}*/
\'([^\'\\\n]|\\.){2,}\' {
    fprintf(diag_out,
        "LEXICAL ERROR: invalid character literal %s at line %d\n",
        yytext,
        yylineno-1);

        has_error = 1;

        return processToken(TokenType::CharacterLiteral, CHAR_LITERAL);
        }

\'([^\'\\\n]|\\.)*\n {
    fprintf(diag_out,
        "LEXICAL ERROR: unterminated character literal %.*s at line %d\n",
        yyleng,
        yytext,
        yylineno-1
    );

    has_error = 1;
}


    /*  KEYWORDS  */
"auto"      { return processToken(TokenType::KwAuto, KW_AUTO); }
"break"     { return processToken(TokenType::KwBreak, KW_BREAK); }
"bool"      { return processToken(TokenType::KwBool, KW_BOOL); }
"case"      { return processToken(TokenType::KwCase, KW_CASE); }
"char"      { return processToken(TokenType::KwChar, KW_CHAR); }
"class"     { return processToken(TokenType::KwClass, KW_CLASS); }
"const"     { return processToken(TokenType::KwConst, KW_CONST); }
"continue"  { return processToken(TokenType::KwContinue, KW_CONTINUE); }
"default"   { return processToken(TokenType::KwDefault, KW_DEFAULT); }
"delete"    { return processToken(TokenType::KwDelete, KW_DELETE); }
"do"        { return processToken(TokenType::KwDo, KW_DO); }
"double"    { return processToken(TokenType::KwDouble, KW_DOUBLE); }
"else"      { return processToken(TokenType::KwElse, KW_ELSE); }
"enum"      { return processToken(TokenType::KwEnum, KW_ENUM); }
"extern"    { return processToken(TokenType::KwExtern, KW_EXTERN); }
"false"     { return processToken(TokenType::KwFalse, KW_FALSE); }
"float"     { return processToken(TokenType::KwFloat, KW_FLOAT); }
"for"       { return processToken(TokenType::KwFor, KW_FOR); }
"goto"      { return processToken(TokenType::KwGoto, KW_GOTO); }
"if"        { return processToken(TokenType::KwIf, KW_IF); }
"inline"    { return processToken(TokenType::KwInline, KW_INLINE); }
"int"       { return processToken(TokenType::KwInt, KW_INT); }
"long"      { return processToken(TokenType::KwLong, KW_LONG); }
"new"       { return processToken(TokenType::KwNew, KW_NEW); }
"public"    { return processToken(TokenType::KwPublic, KW_PUBLIC); }
"private"   { return processToken(TokenType::KwPrivate, KW_PRIVATE); }
"protected" { return processToken(TokenType::KwProtected, KW_PROTECTED); }
"register"  { return processToken(TokenType::KwRegister, KW_REGISTER); }
"restrict"  { return processToken(TokenType::KwRestrict, KW_RESTRICT); }
"return"    { return processToken(TokenType::KwReturn, KW_RETURN); }
"short"     { return processToken(TokenType::KwShort, KW_SHORT); }
"signed"    { return processToken(TokenType::KwSigned, KW_SIGNED); }
"sizeof"    { return processToken(TokenType::KwSizeof, KW_SIZEOF); }
"static"    { return processToken(TokenType::KwStatic, KW_STATIC); }
"struct"    { return processToken(TokenType::KwStruct, KW_STRUCT); }
"switch"    { return processToken(TokenType::KwSwitch, KW_SWITCH); }
"this"      { return processToken(TokenType::KwThis, KW_THIS); }
"true"      { return processToken(TokenType::KwTrue, KW_TRUE); }
"typedef"   { return processToken(TokenType::KwTypedef, KW_TYPEDEF); }
"union"     { return processToken(TokenType::KwUnion, KW_UNION); }
"unsigned"  { return processToken(TokenType::KwUnsigned, KW_UNSIGNED); }
"void"      { return processToken(TokenType::KwVoid, KW_VOID); }
"volatile"  { return processToken(TokenType::KwVolatile, KW_VOLATILE); }
"while"     { return processToken(TokenType::KwWhile, KW_WHILE); }


    /*  IDENTIFIERS  */
[a-zA-Z_][a-zA-Z0-9_]* {
    if (!expand_macro(yytext)) {
        /* the parser says which names are struct / class / union / enum names, so that "Point p;" is a declaration */
        if (!afterType && type_name_hook && type_name_hook(yytext)) return processToken(TokenType::Identifier, TYPE_NAME);
        return processToken(TokenType::Identifier, IDENTIFIER);
    }
    /* a macro: its text was pushed back and is scanned next */
}


    /*  MULTI-CHARACTER OPERATORS  */

"<<"= { return processToken(TokenType::ShiftLeftAssign, SHIFT_LEFT_ASSIGN); }
">>"= { return processToken(TokenType::ShiftRightAssign, SHIFT_RIGHT_ASSIGN); }
"+="  { return processToken(TokenType::PlusAssign, PLUS_ASSIGN); }
"-="  { return processToken(TokenType::MinusAssign, MINUS_ASSIGN); }
"*="  { return processToken(TokenType::MultiplyAssign, MULTIPLY_ASSIGN); }
"/="  { return processToken(TokenType::DivideAssign, DIVIDE_ASSIGN); }
"%="  { return processToken(TokenType::ModuloAssign, MODULO_ASSIGN); }
"&="  { return processToken(TokenType::BitwiseAndAssign, BITWISE_AND_ASSIGN); }
"|="  { return processToken(TokenType::BitwiseOrAssign, BITWISE_OR_ASSIGN); }
"^="  { return processToken(TokenType::BitwiseXorAssign, BITWISE_XOR_ASSIGN); }
"=="  { return processToken(TokenType::Equal, EQUAL); }
"!="  { return processToken(TokenType::NotEqual, NOT_EQUAL); }
"<="  { return processToken(TokenType::LessEqual, LESS_EQUAL); }
">="  { return processToken(TokenType::GreaterEqual, GREATER_EQUAL); }
"&&"  { return processToken(TokenType::LogicalAnd, LOGICAL_AND); }
"||"  { return processToken(TokenType::LogicalOr, LOGICAL_OR); }
"++"  { return processToken(TokenType::Increment, INCREMENT); }
"--"  { return processToken(TokenType::Decrement, DECREMENT); }
"<<"  { return processToken(TokenType::ShiftLeft, SHIFT_LEFT); }
">>"  { return processToken(TokenType::ShiftRight, SHIFT_RIGHT); }
"->"  { return processToken(TokenType::Arrow, ARROW); }


    /*  SINGLE-CHAR OPERATORS  */

"+"  { return processToken(TokenType::Plus, PLUS); }
"-"  { return processToken(TokenType::Minus, MINUS); }
"*"  { return processToken(TokenType::Star, STAR); }
"/"  { return processToken(TokenType::Slash, SLASH); }
"%"  { return processToken(TokenType::Percent, PERCENT); }
"="  { return processToken(TokenType::Assign, ASSIGN); }

"<"  { return processToken(TokenType::Less, LESS); }
">"  { return processToken(TokenType::Greater, GREATER); }

"!"  { return processToken(TokenType::LogicalNot, LOGICAL_NOT); }
"&"  { return processToken(TokenType::BitwiseAnd, BITWISE_AND); }
"|"  { return processToken(TokenType::BitwiseOr, BITWISE_OR); }
"^"  { return processToken(TokenType::BitwiseXor, BITWISE_XOR); }
"~"  { return processToken(TokenType::BitwiseNot, BITWISE_NOT); }


    /* Hexadecimal */
0[xX][0-9a-fA-F]+                         { return processToken(TokenType::HexLiteral, HEX_LITERAL); }

    /* Floating point */
[0-9]*\.[0-9]+([eE][+-]?[0-9]+)?[fFlL]?   { return processToken(TokenType::FloatLiteral, FLOAT_LITERAL); }
[0-9]+\.([eE][+-]?[0-9]+)?[fFlL]?         { return processToken(TokenType::FloatLiteral, FLOAT_LITERAL); }
[0-9]+[eE][+-]?[0-9]+[fFlL]?              { return processToken(TokenType::FloatLiteral, FLOAT_LITERAL); }

    /* Integer */
[0-9]+                                    { return processToken(TokenType::IntegerLiteral, INTEGER_LITERAL); }


    /*  PUNCTUATORS  */

";" { return processToken(TokenType::Semicolon, SEMICOLON); }
"," { return processToken(TokenType::Comma, COMMA); }
"." { return processToken(TokenType::Dot, DOT); }
":" { return processToken(TokenType::Colon, COLON); }
"?" { return processToken(TokenType::Question, QUESTION); }

"(" { return processToken(TokenType::LeftParen, LEFT_PAREN); }
")" { return processToken(TokenType::RightParen, RIGHT_PAREN); }

"[" { return processToken(TokenType::LeftBracket, LEFT_BRACKET); }
"]" { return processToken(TokenType::RightBracket, RIGHT_BRACKET); }

"{" { return processToken(TokenType::LeftBrace, LEFT_BRACE); }
"}" { return processToken(TokenType::RightBrace, RIGHT_BRACE); }


    /*  WHITESPACE  */
[ \t\n\r]+              {   /* Ignore whitespace */ }


    /*  LEXICAL ERRORS  */

. {
    fprintf(
        diag_out,
        "LEXICAL ERROR: '%s' at line %d\n",
        yytext,
        yylineno
    );

    has_error = 1;
}

%%
/* Replace the identifier just read by its macro text: the text is pushed back (last character first) so
   that the scanner reads it next. Returns false if 'name' is not a macro, or if its text mentions the name
   itself (the expansion would never end). */
static bool expand_macro(const char* identifier)
{
    if (macros.empty()) return false;
    std::string name = identifier;
    std::map<std::string, std::string>::iterator m = macros.find(name);
    if (m == macros.end()) return false;
    if (macroExpansions >= 100000 || m->second.find(name) != std::string::npos) return false;
    std::string text = m->second;
    ++macroExpansions;
    unput(' ');
    for (size_t k = text.size(); k-- > 0;) unput(text[k]);
    unput(' ');
    return true;
}
