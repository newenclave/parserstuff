#pragma once
#include <sstream>

#include "ast.h"
#include "parser.h"
#include "rule_lexem.h"

namespace erules {

template <typename LexemT>
class rule_parser {
public:
    using lexem_type = LexemT;
    using char_type = typename lexem_type::char_type;
    using node_uptr = typename objects::ast::node<lexem_type>::uptr;
    using parser_type = parser<node_uptr, lexem_type>;
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
            constants::token_type::PLUS,
            static_cast<int>(constants::precedence_type::SUM));
        parser_.set_precedense(
            constants::token_type::MINUS,
            static_cast<int>(constants::precedence_type::SUM));

        parser_.set_precedense(
            constants::token_type::MUL,
            static_cast<int>(constants::precedence_type::MUL));
        parser_.set_precedense(
            constants::token_type::DIV,
            static_cast<int>(constants::precedence_type::MUL));
        parser_.set_precedense(
            constants::token_type::MOD,
            static_cast<int>(constants::precedence_type::MUL));

        parser_.set_precedense(
            constants::token_type::DOT,
            static_cast<int>(constants::precedence_type::DOT));
        parser_.set_precedense(
            constants::token_type::DOTDOT,
            static_cast<int>(constants::precedence_type::DOTDOT));
        parser_.set_precedense(
            constants::token_type::DOTDOTDOT,
            static_cast<int>(constants::precedence_type::DOTDOT));

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

    node_uptr parse()
    {
        return parser_.parse_expression(
            static_cast<int>(constants::precedence_type::LOWEST));
    }

private:
    void fill_parsers()
    {
        auto parse_value = [](auto parser_ptr) {
            auto value = parser_ptr->current();
            return std::make_unique<objects::ast::value<lexem_type>>(value);
        };
        parser_.set_nud(constants::token_type::NUMBER, parse_value);
        parser_.set_nud(constants::token_type::FLOAT, parse_value);
        parser_.set_nud(constants::token_type::STRING, parse_value);

        parser_.set_nud(constants::token_type::BOOL_FALSE, parse_value);
        parser_.set_nud(constants::token_type::BOOL_TRUE, parse_value);

        parser_.set_nud(constants::token_type::IDENT, [](auto parser_ptr) {
            auto value = parser_ptr->current();
            return std::make_unique<objects::ast::ident<lexem_type>>(value);
        });

        auto prefix_operation = [this](auto parser_ptr) {
            auto precedence
                = static_cast<int>(constants::precedence_type::PREFIX);
            auto operation = parser_ptr->current();
            parser_ptr->advance();
            return std::make_unique<objects::ast::prefix_operation<lexem_type>>(
                operation, parser_ptr->parse_expression(precedence));
        };
        parser_.set_nud(constants::token_type::NOT, prefix_operation);
        parser_.set_nud(constants::token_type::MINUS, prefix_operation);
        parser_.set_nud(constants::token_type::PLUS, prefix_operation);

        auto binary_operation = [this](auto parser_ptr, node_uptr left) {
            auto current = parser_ptr->current();
            auto pp = parser_ptr->current_precednse();
            parser_ptr->advance();
            auto right = parser_ptr->parse_expression(pp);
            return std::make_unique<objects::ast::binary_operation<lexem_type>>(
                std::move(current), std::move(left), std::move(right));
        };

        parser_.set_led(constants::token_type::PLUS, binary_operation);
        parser_.set_led(constants::token_type::MINUS, binary_operation);
        parser_.set_led(constants::token_type::MUL, binary_operation);
        parser_.set_led(constants::token_type::DIV, binary_operation);
        parser_.set_led(constants::token_type::MOD, binary_operation);

        parser_.set_led(constants::token_type::DOT, binary_operation);
        parser_.set_led(constants::token_type::DOTDOT, binary_operation);
        parser_.set_led(constants::token_type::DOTDOTDOT, binary_operation);

        parser_.set_led(constants::token_type::IN, binary_operation);
        parser_.set_led(constants::token_type::EQ, binary_operation);
        parser_.set_led(constants::token_type::NOTEQ, binary_operation);
        parser_.set_led(constants::token_type::LT, binary_operation);
        parser_.set_led(constants::token_type::GT, binary_operation);
        parser_.set_led(constants::token_type::OR, binary_operation);
        parser_.set_led(constants::token_type::AND, binary_operation);
        parser_.set_led(constants::token_type::GEQ, binary_operation);
        parser_.set_led(constants::token_type::LEQ, binary_operation);

        parser_.set_nud(constants::token_type::LPAREN, [this](auto parser_ptr) {
            parser_.advance();
            auto expr = parser_ptr->parse_expression(
                static_cast<int>(constants::precedence_type::LOWEST));
            parser_ptr->expect(constants::token_type::RPAREN);
            return expr;
        });
    }

    parser_type parser_;
};

}
