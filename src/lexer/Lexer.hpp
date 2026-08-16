#pragma once

#include <iostream>
#include <vector>
#include <string>

#ifdef yyFlexLexer
#undef yyFlexLexer
#endif

#include <FlexLexer.h>

#include "Token.hpp"

struct LexicalError {
    std::string message;
    int line;
    int column;
};

class Lexer : public yyFlexLexer {
public:
    explicit Lexer(std::istream* input);

    int yylex() override;

    Token nextToken();

    const std::vector<LexicalError>& errors() const;

    Token currentToken;

private:
    std::vector<LexicalError> lexicalErrors;

    void addError(const std::string& message, int line, int column);
};