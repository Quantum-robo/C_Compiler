#include "Token.hpp"

const char* tokenTypeToString(TokenType type)
{
    switch (type)
    {
        // Keywords
        case TokenType::KwAuto:        return "TOKEN_KW_AUTO";
        case TokenType::KwBreak:       return "TOKEN_KW_BREAK";
        case TokenType::KwCase:        return "TOKEN_KW_CASE";
        case TokenType::KwChar:        return "TOKEN_KW_CHAR";
        case TokenType::KwClass:       return "TOKEN_KW_CLASS";
        case TokenType::KwConst:       return "TOKEN_KW_CONST";
        case TokenType::KwContinue:    return "TOKEN_KW_CONTINUE";
        case TokenType::KwDefault:     return "TOKEN_KW_DEFAULT";
        case TokenType::KwDelete:      return "TOKEN_KW_DELETE";
        case TokenType::KwDo:          return "TOKEN_KW_DO";
        case TokenType::KwDouble:      return "TOKEN_KW_DOUBLE";
        case TokenType::KwElse:        return "TOKEN_KW_ELSE";
        case TokenType::KwEnum:        return "TOKEN_KW_ENUM";
        case TokenType::KwExtern:      return "TOKEN_KW_EXTERN";
        case TokenType::KwFloat:       return "TOKEN_KW_FLOAT";
        case TokenType::KwFor:         return "TOKEN_KW_FOR";
        case TokenType::KwGoto:        return "TOKEN_KW_GOTO";
        case TokenType::KwIf:          return "TOKEN_KW_IF";
        case TokenType::KwInline:      return "TOKEN_KW_INLINE";
        case TokenType::KwInt:         return "TOKEN_KW_INT";
        case TokenType::KwLong:        return "TOKEN_KW_LONG";
        case TokenType::KwNew:         return "TOKEN_KW_NEW";
        case TokenType::KwPrivate:     return "TOKEN_KW_PRIVATE";
        case TokenType::KwProtected:   return "TOKEN_KW_PROTECTED";
        case TokenType::KwPublic:      return "TOKEN_KW_PUBLIC";
        case TokenType::KwRegister:    return "TOKEN_KW_REGISTER";
        case TokenType::KwRestrict:    return "TOKEN_KW_RESTRICT";
        case TokenType::KwReturn:      return "TOKEN_KW_RETURN";
        case TokenType::KwShort:       return "TOKEN_KW_SHORT";
        case TokenType::KwSigned:      return "TOKEN_KW_SIGNED";
        case TokenType::KwSizeof:      return "TOKEN_KW_SIZEOF";
        case TokenType::KwStatic:      return "TOKEN_KW_STATIC";
        case TokenType::KwStruct:      return "TOKEN_KW_STRUCT";
        case TokenType::KwSwitch:      return "TOKEN_KW_SWITCH";
        case TokenType::KwThis:        return "TOKEN_KW_THIS";
        case TokenType::KwTypedef:     return "TOKEN_KW_TYPEDEF";
        case TokenType::KwUnion:       return "TOKEN_KW_UNION";
        case TokenType::KwUnsigned:    return "TOKEN_KW_UNSIGNED";
        case TokenType::KwVoid:        return "TOKEN_KW_VOID";
        case TokenType::KwVolatile:    return "TOKEN_KW_VOLATILE";
        case TokenType::KwWhile:       return "TOKEN_KW_WHILE";

        // Identifier
        case TokenType::Identifier:     return "TOKEN_IDENTIFIER";

        // Literals 
        case TokenType::IntegerLiteral:   return "TOKEN_INTEGER_LITERAL";
        case TokenType::HexLiteral:       return "Token_Hex_Literal";
        case TokenType::FloatLiteral:     return "TOKEN_FLOAT_LITERAL";
        case TokenType::CharacterLiteral: return "TOKEN_CHAR_LITERAL";
        case TokenType::StringLiteral:    return "TOKEN_STRING_LITERAL";

        // Arithmetic
        case TokenType::Plus:    return "TOKEN_PLUS";
        case TokenType::Minus:   return "TOKEN_MINUS";
        case TokenType::Star:    return "TOKEN_STAR";
        case TokenType::Slash:   return "TOKEN_SLASH";
        case TokenType::Percent: return "TOKEN_PERCENT";

        // Assignment
        case TokenType::Assign:            return "TOKEN_ASSIGN";
        case TokenType::PlusAssign:        return "TOKEN_PLUS_ASSIGN";
        case TokenType::MinusAssign:       return "TOKEN_MINUS_ASSIGN";
        case TokenType::MultiplyAssign:    return "TOKEN_MULTIPLY_ASSIGN";
        case TokenType::DivideAssign:      return "TOKEN_DIVIDE_ASSIGN";
        case TokenType::ModuloAssign:      return "TOKEN_MODULO_ASSIGN";
        case TokenType::BitwiseAndAssign:  return "TOKEN_BITWISE_AND_ASSIGN";
        case TokenType::BitwiseOrAssign:   return "TOKEN_BITWISE_OR_ASSIGN";
        case TokenType::BitwiseXorAssign:  return "TOKEN_BITWISE_XOR_ASSIGN";


        // Comparison
        case TokenType::Equal:             return "TOKEN_EQUAL";
        case TokenType::NotEqual:          return "TOKEN_NOT_EQUAL";
        case TokenType::Less:              return "TOKEN_LESS";
        case TokenType::LessEqual:         return "TOKEN_LESS_EQUAL";
        case TokenType::Greater:           return "TOKEN_GREATER";
        case TokenType::GreaterEqual:      return "TOKEN_GREATER_EQUAL";

        // Logical
        case TokenType::LogicalAnd:        return "TOKEN_LOGICAL_AND";
        case TokenType::LogicalOr:         return "TOKEN_LOGICAL_OR";
        case TokenType::LogicalNot:        return "TOKEN_LOGICAL_NOT";

        // Bitwise
        case TokenType::BitwiseAnd:        return "TOKEN_BITWISE_AND";
        case TokenType::BitwiseOr:         return "TOKEN_BITWISE_OR";
        case TokenType::BitwiseXor:        return "TOKEN_BITWISE_XOR";
        case TokenType::BitwiseNot:        return "TOKEN_BITWISE_NOT";

        // Increment / decrement
        case TokenType::Increment:          return "TOKEN_INCREMENT";
        case TokenType::Decrement:          return "TOKEN_DECREMENT";

        // Shift
        case TokenType::ShiftLeft:          return "TOKEN_SHIFT_LEFT";
        case TokenType::ShiftRight:         return "TOKEN_SHIFT_RIGHT";
        case TokenType::ShiftLeftAssign:    return "TOKEN_SHIFT_LEFT_ASSIGN";
        case TokenType::ShiftRightAssign:   return "TOKEN_SHIFT_RIGHT_ASSIGN";

        // Other
        case TokenType::Arrow:              return "TOKEN_ARROW";

        // Punctuators
        case TokenType::Semicolon:          return "TOKEN_SEMICOLON";
        case TokenType::Comma:              return "TOKEN_COMMA";
        case TokenType::Dot:                return "TOKEN_DOT";
        case TokenType::Colon:              return "TOKEN_COLON";
        case TokenType::Question:           return "TOKEN_QUESTION";
        case TokenType::LeftParen:          return "TOKEN_LEFT_PAREN";
        case TokenType::RightParen:         return "TOKEN_RIGHT_PAREN";
        case TokenType::LeftBracket:        return "TOKEN_LEFT_BRACKET";
        case TokenType::RightBracket:       return "TOKEN_RIGHT_BRACKET";
        case TokenType::LeftBrace:          return "TOKEN_LEFT_BRACE";
        case TokenType::RightBrace:         return "TOKEN_RIGHT_BRACE";

        // Special
        case TokenType::EndOfFile:          return "TOKEN_EOF";
        case TokenType::Unknown:            return "TOKEN_UNKNOWN";
    }

    return "TOKEN_UNKNOWN";
}