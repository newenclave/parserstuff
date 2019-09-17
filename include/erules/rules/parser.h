#pragma once
#include "erules/common/parser_base.h"
#include "erules/rules/ast.h"
#include "erules/rules/lexer.h"

namespace erules { namespace rules {

    template <typename CharT, typename KeyT, typename LessT = std::less<CharT>>
    class parser {
    public:
        using char_type = CharT;
        using key_type = KeyT;
        using less_type = LessT;
        using base_parser = common::parser_base<char_type, key_type, less_type>;
        using lexer_type = lexer<char_type, key_type, less_type>;

    private:
        base_parser parser_;
        lexer_type lexer_;
    };
}}