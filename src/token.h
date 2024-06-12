#pragma once
#include <string>
#include <optional>

enum class TokenType {
    IntegerLiteral, // 8909
    StringLiteral, // "str"
    FloatLiteral, // 45.7
    BoolLiteral, // false

    BoolType, // bool
    StringType, // string
    IntType, // int
    FloatType, // float

    Error,
    EOI, // '\0'
    NonTerminal,
    NewLine, // \n
    Identifier, // variable_name, function_name

    Semicolon, // ;
    Colon, // :
    And, // &&
    Or, // ||
    Not, // !
    Equal, // ==
    Assign, // =
    Comma, // ,
    Dot, // .
    Plus, // +
    Minus, // -
    Asterisk, // *
    Slash, // /
    BackSlash,
    Greater, // >
    Less, // <
    Percent, // %
    LParent, // (
    RParent, // )
    LBracket, // [
    RBracket, // ]
    LBrace, // {
    RBrace, // }
    NotEqual, // !=
    AsteriskEqual, // *=
    SlashEqual, // /=
    PlusEqual, // +=
    MinusEqual, // -=

    If,
    Else,
    While,
    For,
    In,
    Do,
    Var,
    Const,
    Foreach,
    Fn,
};

struct Token {
    TokenType type;
    std::optional<int> int_val;
    std::optional<float> float_val;
    std::optional<std::string> string_val;
    std::optional<bool> bool_val;

    Token() {
        this->type = TokenType::Error;
    }

    Token(TokenType type) {
        this->type = type;
    }

    Token(TokenType type, std::optional<int> int_val, std::optional<float> float_val,
          std::optional<std::string> string_val, std::optional<bool> bool_val) {
        this->type = type;
        this->int_val = int_val;
        this->float_val = float_val;
        this->string_val = string_val;
        this->bool_val = bool_val;
    }

    bool operator==(Token token)
    {
        return type == token.type && int_val == token.int_val && float_val == token.float_val &&
            string_val == token.string_val && bool_val == token.bool_val;
    }

    static Token FloatLiteral(float value) {
        return {
            TokenType::FloatLiteral,
            std::nullopt,
            value,
            std::nullopt,
            std::nullopt
        };
    }

    static Token IntegerLiteral(int value) {
        return {
                TokenType::IntegerLiteral,
                value,
                std::nullopt,
                std::nullopt,
                std::nullopt
        };
    }

    static Token StringLiteral(std::string value) {
        return {
                TokenType::StringLiteral,
                std::nullopt,
                std::nullopt,
                value,
                std::nullopt
        };
    }

    static Token BoolLiteral(bool value) {
        return {
                TokenType::BoolLiteral,
                std::nullopt,
                std::nullopt,
                std::nullopt,
                value
        };
    }

    static Token Identifier(std::string value) {
        return {
                TokenType::Identifier,
                std::nullopt,
                std::nullopt,
                value,
                std::nullopt
        };
    }

    static Token Error(std::string value) {
        return {
                TokenType::Error,
                std::nullopt,
                std::nullopt,
                value,
                std::nullopt
        };
    }

    static Token NonTerminal(std::string value) {
        return {
                TokenType::NonTerminal,
                std::nullopt,
                std::nullopt,
                value,
                std::nullopt
        };
    }
};