#pragma once
#include <string>
#include "erules/helpers.h"

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
        return helpers::strings::to_string<CharT>("none");
    case constants::token_type::AND:
        return helpers::strings::to_string<CharT>("and");
    case constants::token_type::OR:
        return helpers::strings::to_string<CharT>("or");
    case constants::token_type::EQ:
        return helpers::strings::to_string<CharT>("==");
    case constants::token_type::LT:
        return helpers::strings::to_string<CharT>("<");
    case constants::token_type::GT:
        return helpers::strings::to_string<CharT>(">");
    case constants::token_type::LEQ:
        return helpers::strings::to_string<CharT>("<=");
    case constants::token_type::GEQ:
        return helpers::strings::to_string<CharT>(">=");
    case constants::token_type::NOT:
        return helpers::strings::to_string<CharT>("not");
    case constants::token_type::NOTEQ:
        return helpers::strings::to_string<CharT>("<>");
    case constants::token_type::IN:
        return helpers::strings::to_string<CharT>("in");
    case constants::token_type::IDENT:
        return helpers::strings::to_string<CharT>("ident");
    case constants::token_type::NUMBER:
        return helpers::strings::to_string<CharT>("number");
    case constants::token_type::FLOAT:
        return helpers::strings::to_string<CharT>("float");
    case constants::token_type::STRING:
        return helpers::strings::to_string<CharT>("string");
    case constants::token_type::LPAREN:
        return helpers::strings::to_string<CharT>("(");
    case constants::token_type::RPAREN:
        return helpers::strings::to_string<CharT>(")");
    case constants::token_type::LBRACE:
        return helpers::strings::to_string<CharT>("[");
    case constants::token_type::RBRACE:
        return helpers::strings::to_string<CharT>("]");
    case constants::token_type::BOOL_FALSE:
        return helpers::strings::to_string<CharT>("false");
    case constants::token_type::BOOL_TRUE:
        return helpers::strings::to_string<CharT>("true");
    case constants::token_type::SEMICOLON:
        return helpers::strings::to_string<CharT>(";");
    default:
        break;
    }
    return helpers::strings::to_string<CharT>("");
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
