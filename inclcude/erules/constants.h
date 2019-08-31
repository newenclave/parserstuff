#pragma once
#include <string>

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

    BOOL_FALSE,
    BOOL_TRUE,

    SEMICOLON,
};

template <typename CharT>
inline std::basic_string<CharT> token_to_string(token_type t)
{
    switch (t) {
    case constants::token_type::NONE:
        return helpers::strings::to_string("none");
    case constants::token_type::AND:
        return helpers::strings::to_string("and");
    case constants::token_type::OR:
        return helpers::strings::to_string("or");
    case constants::token_type::EQ:
        return helpers::strings::to_string("==");
    case constants::token_type::LT:
        return helpers::strings::to_string("<");
    case constants::token_type::GT:
        return helpers::strings::to_string(">");
    case constants::token_type::LEQ:
        return helpers::strings::to_string("<=");
    case constants::token_type::GEQ:
        return helpers::strings::to_string(">=");
    case constants::token_type::NOT:
        return helpers::strings::to_string("not");
    case constants::token_type::NOTEQ:
        return helpers::strings::to_string("<>");
    case constants::token_type::IN:
        return helpers::strings::to_string("in");
    case constants::token_type::IDENT:
        return helpers::strings::to_string("ident");
    case constants::token_type::NUMBER:
        return helpers::strings::to_string("number");
    case constants::token_type::FLOAT:
        return helpers::strings::to_string("float");
    case constants::token_type::STRING:
        return helpers::strings::to_string("string");
    case constants::token_type::LPAREN:
        return helpers::strings::to_string("(");
    case constants::token_type::RPAREN:
        return helpers::strings::to_string(")");
    case constants::token_type::LBRACE:
        return helpers::strings::to_string("[");
    case constants::token_type::RBRACE:
        return helpers::strings::to_string("]");
    case constants::token_type::BOOL_FALSE:
        return helpers::strings::to_string("false");
    case constants::token_type::BOOL_TRUE:
        return helpers::strings::to_string("true");
    case constants::token_type::SEMICOLON:
        return helpers::strings::to_string(";");
    default:
        break;
    }
    return helpers::strings::to_string("");
}

enum class precedence_type : int {
    LOWEST = -1,
    LOR = 1,
    LAND = 2,
    CMP = 3,
    IN = 4,
    PAREN = 10,
    PREFIX = 100,
};
}
