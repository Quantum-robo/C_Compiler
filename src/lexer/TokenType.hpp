#pragma once

enum class TokenType {
    Keyword,
    Identifier,

    IntegerLiteral,
    FloatLiteral,
    HexLiteral,

    StringLiteral,
    CharacterLiteral,

    Operator,
    Separator,

    EndOfFile,
    Unknown
};

