#pragma once

#include <string>
#include "TokenType.hpp"

struct Token {
    TokenType type;
    std::string lexeme;
};

const char* tokenTypeToString(TokenType type);