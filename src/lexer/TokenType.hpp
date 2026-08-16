#pragma once

enum class TokenType {

    // KEYWORDS 

    KwAuto,
    KwBreak,
    KwBool,
    KwCase,
    KwChar,
    KwConst,
    KwContinue,
    KwClass,
    KwDefault,
    KwDelete,
    KwDo,
    KwDouble,
    KwElse,
    KwEnum,
    KwExtern,
    KwFalse,
    KwFloat,
    KwFor,
    KwGoto,
    KwIf,
    KwInline,
    KwInt,
    KwLong,
    KwNew,
    KwPublic,
    KwPrivate,
    KwProtected,
    KwRegister,
    KwRestrict,
    KwReturn,
    KwShort,
    KwSigned,
    KwSizeof,
    KwStatic,
    KwStruct,
    KwSwitch,
    KwThis,
    KwTrue,
    KwTypedef,
    KwUnion,
    KwUnsigned,
    KwVoid,
    KwVolatile,
    KwWhile,

    // IDENTIFIERS

    Identifier,

    // LITERALS 

    IntegerLiteral,
    HexLiteral,
    FloatLiteral,
    CharacterLiteral,
    StringLiteral,

    // ARITHMETIC OPERATORS 

    Plus,
    Minus,
    Star,
    Slash,
    Percent,

    // ASSIGNMENT OPERATORS

    Assign,
    PlusAssign,
    MinusAssign,
    MultiplyAssign,
    DivideAssign,
    ModuloAssign,

    // COMPARISON OPERATORS 

    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,

    //LOGICAL OPERATORS 

    LogicalAnd,
    LogicalOr,
    LogicalNot,

    //  BITWISE OPERATORS 

    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    BitwiseNot,
    BitwiseAndAssign,
    BitwiseOrAssign,
    BitwiseXorAssign,

    //  INCREMENT / DECREMENT
    Increment,
    Decrement,

    //  SHIFT OPERATORS

    ShiftLeft,
    ShiftRight,
    ShiftLeftAssign,
    ShiftRightAssign,

    //  OTHER OPERATORS
    Arrow,

    //  PUNCTUATORS

    Semicolon,
    Comma,
    Dot,
    Colon,
    Question,

    LeftParen,
    RightParen,
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,

    // SPECIAL
    EndOfFile,
    Unknown
};