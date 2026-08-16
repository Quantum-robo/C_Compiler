flex Lexer.l

g++ -std=c++17 lex.yy.c Token.cpp -I. -o lexer

./lexer testfile
