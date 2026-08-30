%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

void yyerror(const char* s);
int has_error = 0;

%}

%union {
    char* str;
}

/* Tokens */
%token KW_AUTO KW_BREAK KW_BOOL KW_CASE KW_CHAR KW_CLASS KW_CONST KW_CONTINUE
%token KW_DEFAULT KW_DELETE KW_DO KW_DOUBLE KW_ELSE KW_ENUM KW_EXTERN KW_FALSE
%token KW_FLOAT KW_FOR KW_GOTO KW_IF KW_INLINE KW_INT KW_LONG KW_NEW
%token KW_PUBLIC KW_PRIVATE KW_PROTECTED KW_REGISTER KW_RESTRICT KW_RETURN
%token KW_SHORT KW_SIGNED KW_SIZEOF KW_STATIC KW_STRUCT KW_SWITCH KW_THIS
%token KW_TRUE KW_TYPEDEF KW_UNION KW_UNSIGNED KW_VOID KW_VOLATILE KW_WHILE

%token IDENTIFIER
%token INTEGER_LITERAL HEX_LITERAL FLOAT_LITERAL CHAR_LITERAL STRING_LITERAL

%token PLUS MINUS STAR SLASH PERCENT
%token ASSIGN PLUS_ASSIGN MINUS_ASSIGN MULTIPLY_ASSIGN DIVIDE_ASSIGN MODULO_ASSIGN
%token BITWISE_AND_ASSIGN BITWISE_OR_ASSIGN BITWISE_XOR_ASSIGN
%token EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT
%token BITWISE_AND BITWISE_OR BITWISE_XOR BITWISE_NOT
%token INCREMENT DECREMENT SHIFT_LEFT SHIFT_RIGHT
%token SHIFT_LEFT_ASSIGN SHIFT_RIGHT_ASSIGN
%token ARROW

%token SEMICOLON COMMA DOT COLON QUESTION
%token LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET LEFT_BRACE RIGHT_BRACE

/* Precedence rules */
%right ASSIGN
%left EQUAL NOT_EQUAL
%left LESS GREATER LESS_EQUAL GREATER_EQUAL
%left PLUS MINUS
%left STAR SLASH PERCENT
%nonassoc INCREMENT DECREMENT

%start program

%%

program
    : external_declaration_list
    ;

external_declaration_list
    : external_declaration
    | external_declaration_list external_declaration
    ;

/* Added class and struct to external declarations */
external_declaration
    : function_definition
    | declaration
    | class_definition
    | struct_definition
    ;

/* --- CLASSES & STRUCTS --- */
class_definition
    : KW_CLASS IDENTIFIER LEFT_BRACE class_member_list_opt RIGHT_BRACE SEMICOLON
    ;

struct_definition
    : KW_STRUCT IDENTIFIER LEFT_BRACE struct_member_list_opt RIGHT_BRACE SEMICOLON
    ;

class_member_list_opt
    : /* empty */
    | class_member_list
    ;

class_member_list
    : class_member
    | class_member_list class_member
    ;

/* Class members can have access modifiers (public:), variables, or functions */
class_member
    : access_specifier COLON
    | declaration
    | function_definition
    ;

struct_member_list_opt
    : /* empty */
    | struct_member_list
    ;

struct_member_list
    : declaration
    | struct_member_list declaration
    ;

access_specifier
    : KW_PUBLIC
    | KW_PRIVATE
    | KW_PROTECTED
    ;

/* --- FUNCTIONS --- */
function_definition
    : declaration_specifiers IDENTIFIER LEFT_PAREN parameter_list_opt RIGHT_PAREN compound_statement
    ;

parameter_list_opt
    : /* empty */
    | parameter_list
    ;

parameter_list
    : parameter_declaration
    | parameter_list COMMA parameter_declaration
    ;

parameter_declaration
    : declaration_specifiers IDENTIFIER
    ;

/* --- STATEMENTS & BLOCKS --- */
compound_statement
    : LEFT_BRACE block_item_list_opt RIGHT_BRACE
    ;

block_item_list_opt
    : /* empty */
    | block_item_list
    ;

block_item_list
    : block_item
    | block_item_list block_item
    ;

block_item
    : declaration
    | statement
    ;

declaration
    : declaration_specifiers IDENTIFIER ASSIGN expression SEMICOLON
    | declaration_specifiers IDENTIFIER SEMICOLON
    ;

declaration_specifiers
    : type_specifier
    | KW_CONST type_specifier
    ;

type_specifier
    : KW_INT | KW_FLOAT | KW_CHAR | KW_BOOL | KW_DOUBLE | KW_VOID
    ;


statement
    : selection_statement
    | iteration_statement
    | jump_statement
    | expression_statement
    | compound_statement
    ;

selection_statement
    : KW_IF LEFT_PAREN expression RIGHT_PAREN statement
    | KW_IF LEFT_PAREN expression RIGHT_PAREN statement KW_ELSE statement
    ;

iteration_statement
    : KW_WHILE LEFT_PAREN expression RIGHT_PAREN statement
    | KW_FOR LEFT_PAREN expression_opt SEMICOLON expression_opt SEMICOLON expression_opt RIGHT_PAREN statement
    | KW_FOR LEFT_PAREN declaration expression_opt SEMICOLON expression_opt RIGHT_PAREN statement
    ;

jump_statement
    : KW_BREAK SEMICOLON
    | KW_CONTINUE SEMICOLON
    | KW_RETURN expression_opt SEMICOLON
    ;

expression_statement
    : expression_opt SEMICOLON
    ;

expression_opt
    : /* empty */
    | expression
    ;

/* Added support for `this->value` in expressions */
expression
    : IDENTIFIER
    | INTEGER_LITERAL
    | FLOAT_LITERAL
    | CHAR_LITERAL
    | KW_TRUE
    | KW_FALSE
    | KW_THIS ARROW IDENTIFIER
    | KW_THIS ARROW IDENTIFIER ASSIGN expression
    | expression ASSIGN expression
    | expression EQUAL expression
    | expression NOT_EQUAL expression
    | expression LESS expression
    | expression GREATER expression
    | expression PLUS expression
    | expression MINUS expression
    | expression INCREMENT
    | expression DECREMENT
    ;

%%

void yyerror(const char* s) {
    has_error = 1;
    fprintf(stderr, "SYNTAX ERROR: %s at line %d (near '%s')\n", s, yylineno, yytext);
}

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }
        yyin = file;
    }

    printf("%-20s | %s\n", "Token", "Token_Type");
    printf("---------------------|----------------------\n");

    int res = yyparse();
    
    if (res == 0 && !has_error) {
        printf("\n=> PARSE SUCCESSFUL: No syntax errors found.\n");
    }

    return 0;
}
