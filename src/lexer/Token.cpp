#include "Token.hpp"

const char* tokenTypeToString(TokenType type)
{
    switch (type)
    {
        case TokenType::Keyword:
            return "keyword";

        case TokenType::Identifier:
            return "identifier";

        case TokenType::IntegerLiteral:
            return "integer literal";

        case TokenType::FloatLiteral:
            return "float literal";

        case TokenType::HexLiteral:
            return "hex literal";

        case TokenType::StringLiteral:
            return "string";

        case TokenType::CharacterLiteral:
            return "character";

        case TokenType::Operator:
            return "operator";

        case TokenType::Separator:
            return "separator";

        case TokenType::EndOfFile:
            return "EOF";

        case TokenType::Unknown:
            return "unknown";
    }

    return "unknown";
}