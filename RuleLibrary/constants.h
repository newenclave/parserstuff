#pragma once

namespace constants {
enum class token_type : int {
    NONE,
    AND,
    OR,
    EQ,
    LT,
    GT,
    LEQ,
    GEQ,
    NOT,
    NOTEQ,
    IN,

    IDENT,
    NUMBER,
    FLOAT,
    STRING,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    SEMICOLON,
};

enum class precedence_type : int {
    LOWEST = -1,
    LOR = 1,
    LAND = 2,
    CMP = 3,
    CONTAIN = 4,
    PAREN = 10,
    PREFIX = 100,
};
}
