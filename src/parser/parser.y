%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

struct LexedToken {
    std::string token;
    std::string token_type;
};

extern std::vector<LexedToken> lexed_tokens;

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


/* Dangling-else declarations */
%nonassoc LOWER_THAN_ELSE
%nonassoc KW_ELSE

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
    | declaration_specifiers IDENTIFIER array_suffix SEMICOLON
    ;

    array_suffix
    : LEFT_BRACKET expression RIGHT_BRACKET
    | array_suffix LEFT_BRACKET expression RIGHT_BRACKET
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
    | error SEMICOLON
    ;

    selection_statement
    : KW_IF LEFT_PAREN expression RIGHT_PAREN statement %prec LOWER_THAN_ELSE
    | KW_IF LEFT_PAREN expression RIGHT_PAREN statement KW_ELSE statement
    | KW_IF error statement %prec LOWER_THAN_ELSE
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
    : assignment_expression
    ;

    assignment_expression
    : conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;

    assignment_operator
    : ASSIGN
    | PLUS_ASSIGN
    | MINUS_ASSIGN
    | MULTIPLY_ASSIGN
    | DIVIDE_ASSIGN
    | MODULO_ASSIGN
    | BITWISE_AND_ASSIGN
    | BITWISE_OR_ASSIGN
    | BITWISE_XOR_ASSIGN
    | SHIFT_LEFT_ASSIGN
    | SHIFT_RIGHT_ASSIGN
    ;

    conditional_expression
    : logical_or_expression
    | logical_or_expression QUESTION expression COLON conditional_expression
    ;

    logical_or_expression
    : logical_and_expression
    | logical_or_expression LOGICAL_OR logical_and_expression
    ;

    logical_and_expression
    : bitwise_or_expression
    | logical_and_expression LOGICAL_AND bitwise_or_expression
    ;

    bitwise_or_expression
    : bitwise_xor_expression
    | bitwise_or_expression BITWISE_OR bitwise_xor_expression
    ;

    bitwise_xor_expression
    : bitwise_and_expression
    | bitwise_xor_expression BITWISE_XOR bitwise_and_expression
    ;

    bitwise_and_expression
    : equality_expression
    | bitwise_and_expression BITWISE_AND equality_expression
    ;

    equality_expression
    : relational_expression
    | equality_expression EQUAL relational_expression
    | equality_expression NOT_EQUAL relational_expression
    ;

    relational_expression
    : shift_expression
    | relational_expression LESS shift_expression
    | relational_expression LESS_EQUAL shift_expression
    | relational_expression GREATER shift_expression
    | relational_expression GREATER_EQUAL shift_expression
    ;

    shift_expression
    : additive_expression
    | shift_expression SHIFT_LEFT additive_expression
    | shift_expression SHIFT_RIGHT additive_expression
    ;

    additive_expression
    : multiplicative_expression
    | additive_expression PLUS multiplicative_expression
    | additive_expression MINUS multiplicative_expression
    ;

    multiplicative_expression
    : unary_expression
    | multiplicative_expression STAR unary_expression
    | multiplicative_expression SLASH unary_expression
    | multiplicative_expression PERCENT unary_expression
    ;

    unary_expression
    : postfix_expression
    | PLUS unary_expression
    | MINUS unary_expression
    | LOGICAL_NOT unary_expression
    | BITWISE_NOT unary_expression
    | BITWISE_AND unary_expression
    | STAR unary_expression
    | INCREMENT unary_expression
    | DECREMENT unary_expression
    ;

    postfix_expression
    : primary_expression
    | postfix_expression INCREMENT
    | postfix_expression DECREMENT
    | postfix_expression LEFT_BRACKET expression RIGHT_BRACKET
    | postfix_expression LEFT_PAREN argument_list_opt RIGHT_PAREN
    | postfix_expression DOT IDENTIFIER
    | postfix_expression ARROW IDENTIFIER
    ;

    primary_expression
    : IDENTIFIER
    | INTEGER_LITERAL
    | HEX_LITERAL
    | FLOAT_LITERAL
    | CHAR_LITERAL
    | STRING_LITERAL
    | KW_TRUE
    | KW_FALSE
    | KW_THIS
    | LEFT_PAREN expression RIGHT_PAREN
    ;

    argument_list_opt
    : /* empty */
    | argument_list
    ;

    argument_list
    : expression
    | argument_list COMMA expression
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

    int res = yyparse();

if (res == 0 && !has_error) {
    printf("%-20s | %s\n", "Token", "Token_Type");
    printf("---------------------|----------------------\n");

    for (const auto& token : lexed_tokens) {
        printf("%-20s | %s\n",
               token.token.c_str(),
               token.token_type.c_str());
    }

    printf("\n=> PARSE SUCCESSFUL: No syntax errors found.\n");
    return 0;
}

    return 1;
}
