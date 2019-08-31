#pragma once
#include <sstream>

#include "ast.h"
#include "parser.h"
#include "rule_lexem.h"

template <typename LexemT>
class rule_parser {
public:
    using lexem_type = LexemT;
    using char_type = typename lexem_type::char_type;
    using parser_type = parser<typename ast::node<char_type>::uptr, lexem_type>;
    using stream_type = std::basic_stringstream<char_type>;

    rule_parser(rule_parser&&) = delete;
    rule_parser& operator=(rule_parser&&) = delete;
    rule_parser(const rule_parser&) = delete;
    rule_parser& operator=(const rule_parser&) = delete;

    rule_parser(std::vector<lexem_type> lexems)
        : parser_(std::move(lexems))
    {
        parser_.set_precedense(
            constants::token_type::OR,
            static_cast<int>(constants::precedence_type::LOR));
        parser_.set_precedense(
            constants::token_type::AND,
            static_cast<int>(constants::precedence_type::LAND));
        parser_.set_precedense(
            constants::token_type::EQ,
            static_cast<int>(constants::precedence_type::CMP));
        parser_.set_precedense(
            constants::token_type::NOTEQ,
            static_cast<int>(constants::precedence_type::CMP));
        parser_.set_precedense(
            constants::token_type::LT,
            static_cast<int>(constants::precedence_type::CMP));
        parser_.set_precedense(
            constants::token_type::GT,
            static_cast<int>(constants::precedence_type::CMP));
        parser_.set_precedense(
            constants::token_type::LEQ,
            static_cast<int>(constants::precedence_type::CMP));
        parser_.set_precedense(
            constants::token_type::GEQ,
            static_cast<int>(constants::precedence_type::CMP));

        parser_.set_precedense(
            constants::token_type::IN,
            static_cast<int>(constants::precedence_type::IN));
        parser_.set_precedense(
            constants::token_type::LPAREN,
            static_cast<int>(constants::precedence_type::PAREN));

        fill_parsers();
    }

    void reset(std::vector<lexem_type> lexems)
    {
        parser_.reset(std::move(lexems));
    }

private:
    void fill_parsers()
    {
        parser_.set_nud(constants::token_type::NUMBER, [this]() {
            auto value = parser_.current().raw_value();
            return std::make_unique<ast::number<char_type>>(0);
        });
    }

    parser_type parser_;
};
