#pragma once

#include <iostream>

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include "Token.hpp"

class Lexer : public yyFlexLexer {
public:
    explicit Lexer(std::istream* input);

    int yylex() override;

    Token nextToken();

    Token currentToken;
};