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

/* Precedence rules (from reference parser, helps resolve ambiguities) */
%right ASSIGN PLUS_ASSIGN MINUS_ASSIGN MULTIPLY_ASSIGN DIVIDE_ASSIGN MODULO_ASSIGN
        BITWISE_AND_ASSIGN BITWISE_OR_ASSIGN BITWISE_XOR_ASSIGN
        SHIFT_LEFT_ASSIGN SHIFT_RIGHT_ASSIGN
%right QUESTION
%left LOGICAL_OR
%left LOGICAL_AND
%left BITWISE_OR
%left BITWISE_XOR
%left BITWISE_AND
%left EQUAL NOT_EQUAL
%left LESS GREATER LESS_EQUAL GREATER_EQUAL
%left SHIFT_LEFT SHIFT_RIGHT
%left PLUS MINUS
%left STAR SLASH PERCENT

/* Dangling-else resolution */
%nonassoc LOWER_THAN_ELSE
%nonassoc KW_ELSE

%start program

%%

/* ============================================================
   TRANSLATION UNIT
   ============================================================ */

program
    : external_declaration
    | program external_declaration
    ;

external_declaration
    : function_definition
    | declaration
    | class_definition
    | struct_definition
    | SEMICOLON
    | error SEMICOLON { yyerrok; }
    ;

/* ============================================================
   CLASS / STRUCT DEFINITIONS
   ============================================================ */

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
    : struct_member
    | struct_member_list struct_member
    ;

struct_member
    : declaration
    | function_definition
    ;

access_specifier
    : KW_PUBLIC
    | KW_PRIVATE
    | KW_PROTECTED
    ;

/* ============================================================
   DECLARATIONS
   ============================================================ */

declaration
    : declaration_specifiers init_declarator_list_opt SEMICOLON
    | declaration_specifiers error SEMICOLON { yyerrok; }
    ;

declaration_specifiers
    : type_specifier
    | type_qualifier
    | storage_class
    | declaration_specifiers type_specifier
    | declaration_specifiers type_qualifier
    | declaration_specifiers storage_class
    ;

storage_class
    : KW_AUTO
    | KW_EXTERN
    | KW_REGISTER
    | KW_STATIC
    | KW_TYPEDEF
    | KW_INLINE
    ;

type_qualifier
    : KW_CONST
    | KW_VOLATILE
    | KW_RESTRICT
    ;

type_specifier
    : KW_VOID
    | KW_CHAR
    | KW_SHORT
    | KW_INT
    | KW_LONG
    | KW_FLOAT
    | KW_DOUBLE
    | KW_SIGNED
    | KW_UNSIGNED
    | KW_BOOL
    ;

init_declarator_list_opt
    : /* empty */
    | init_declarator_list
    ;

init_declarator_list
    : init_declarator
    | init_declarator_list COMMA init_declarator
    ;

init_declarator
    : declarator
    | declarator ASSIGN initializer
    ;

initializer
    : assignment_expression
    | LEFT_BRACE initializer_list comma_opt RIGHT_BRACE
    ;

initializer_list
    : initializer
    | initializer_list COMMA initializer
    ;

comma_opt
    : /* empty */
    | COMMA
    ;

declarator
    : pointer_opt direct_declarator
    ;

pointer_opt
    : /* empty */
    | STAR pointer_opt
    ;

direct_declarator
    : IDENTIFIER
    | LEFT_PAREN declarator RIGHT_PAREN
    | direct_declarator LEFT_BRACKET expression_opt RIGHT_BRACKET
    | direct_declarator LEFT_PAREN parameter_list_opt RIGHT_PAREN
    ;

/* ============================================================
   FUNCTIONS / PARAMETERS
   ============================================================ */

function_definition
    : declaration_specifiers declarator compound_statement
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
    : declaration_specifiers declarator
    | declaration_specifiers
    ;

/* ============================================================
   STATEMENTS
   ============================================================ */

compound_statement
    : LEFT_BRACE block_items_opt RIGHT_BRACE
    | LEFT_BRACE error RIGHT_BRACE { yyerrok; }
    ;

block_items_opt
    : /* empty */
    | block_items
    ;

block_items
    : block_item
    | block_items block_item
    ;

block_item
    : declaration
    | statement
    ;

statement
    : compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    | labeled_statement
    | error SEMICOLON { yyerrok; }
    ;

labeled_statement
    : IDENTIFIER COLON statement
    | KW_CASE conditional_expression COLON statement
    | KW_DEFAULT COLON statement
    ;

expression_statement
    : expression_opt SEMICOLON
    ;

selection_statement
    : KW_IF LEFT_PAREN expression RIGHT_PAREN statement %prec LOWER_THAN_ELSE
    | KW_IF LEFT_PAREN expression RIGHT_PAREN statement KW_ELSE statement
    | KW_SWITCH LEFT_PAREN expression RIGHT_PAREN statement
    ;

iteration_statement
    : KW_WHILE LEFT_PAREN expression RIGHT_PAREN statement
    | KW_DO statement KW_WHILE LEFT_PAREN expression RIGHT_PAREN SEMICOLON
    | KW_FOR LEFT_PAREN expression_opt SEMICOLON expression_opt SEMICOLON expression_opt RIGHT_PAREN statement
    | KW_FOR LEFT_PAREN declaration expression_opt SEMICOLON expression_opt RIGHT_PAREN statement
    ;

jump_statement
    : KW_GOTO IDENTIFIER SEMICOLON
    | KW_BREAK SEMICOLON
    | KW_CONTINUE SEMICOLON
    | KW_RETURN expression_opt SEMICOLON
    ;

/* ============================================================
   EXPRESSIONS
   ============================================================ */

expression_opt
    : /* empty */
    | expression
    ;

expression
    : assignment_expression
    | expression COMMA assignment_expression
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
    | INCREMENT unary_expression
    | DECREMENT unary_expression
    | BITWISE_AND unary_expression
    | STAR unary_expression
    | PLUS unary_expression
    | MINUS unary_expression
    | BITWISE_NOT unary_expression
    | LOGICAL_NOT unary_expression
    | KW_SIZEOF unary_expression
    ;

postfix_expression
    : primary_expression
    | postfix_expression LEFT_BRACKET expression RIGHT_BRACKET
    | postfix_expression LEFT_PAREN argument_list_opt RIGHT_PAREN
    | postfix_expression DOT IDENTIFIER
    | postfix_expression ARROW IDENTIFIER
    | postfix_expression INCREMENT
    | postfix_expression DECREMENT
    ;

argument_list_opt
    : /* empty */
    | argument_list
    ;

argument_list
    : assignment_expression
    | argument_list COMMA assignment_expression
    ;

primary_expression
    : IDENTIFIER
    | INTEGER_LITERAL
    | HEX_LITERAL
    | FLOAT_LITERAL
    | CHAR_LITERAL
    | string_literal
    | KW_TRUE
    | KW_FALSE
    | KW_THIS
    | LEFT_PAREN expression RIGHT_PAREN
    ;

string_literal
    : STRING_LITERAL
    | string_literal STRING_LITERAL
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
