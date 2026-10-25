#pragma once

#include <string>
#include <vector>
#include "TokenType.hpp"

struct Token {
    TokenType type;
    std::string lexeme;
};

// One row of the token table printed by the standalone lexer (filled in by Lexer.l).
struct LexedToken {
    std::string token;
    std::string token_type;
};

extern std::vector<LexedToken> lexed_tokens;

const char* tokenTypeToString(TokenType type);