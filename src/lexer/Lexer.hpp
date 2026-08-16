#pragma once

#include <iostream>

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif

#include <FlexLexer.h>

#include "Token.hpp"

class Lexer : public yyFlexLexer {
public:
    explicit Lexer(std::istream* input);

    int yylex() override;

    Token nextToken();

    Token currentToken;
};