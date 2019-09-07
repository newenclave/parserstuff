
#include <iostream>
#include <vector>

#include "erules/parser.h"
#include "erules/objects.h"
#include "erules/rule_lexem.h"
#include "erules/rule_lexer.h"
#include "erules/rule_parser.h"

using mlexer = rule_lexer<char>;
using lexem_type = typename mlexer::lexem_type;
using mparser = rule_parser<lexem_type>;
using operations_type = erules::object_operations<typename lexem_type::id_type>;

namespace test_parser {
void run()
{
    operations_type to_string;
    using object = erules::object;
    using string_obj = erules::string<char>;
    namespace east = erules::ast;
    auto make_string = [](std::string val) {
        return std::make_unique<string_obj>(std::move(val));
    };
    auto string_transform = [&to_string](auto obj) {
        return to_string.call_transform<string_obj>(obj);
    };

    to_string.set_transformer<east::ident<lexem_type>, string_obj>(
        [&](auto ident) {
            return make_string(ident->lexem().raw_value());
        });
    to_string.set_transformer<east::value<lexem_type>, string_obj>(
        [&](auto value){
            return make_string(value->lexem().raw_value());
        });
    to_string.set_transformer<east::binary_operation<lexem_type>, string_obj>(
        [&](auto value){
            auto left = string_transform(value->left().get());
            auto right = string_transform(value->right().get());
            return make_string(
                        '(' + left->value() +
                        value->lexem().raw_value() +
                        right->value() + ')');
        });

    std::string val = "a + 2 * 5";
    mlexer lex;
    lex.reset(val);
    auto tokens = lex.read_all(val);

    for (auto& t : tokens) {
        std::cout << t.raw_value() << "\n";
    }
    mparser pars(std::move(tokens));
    auto n = pars.parse();
    auto nc = n->clone();
    std::cout << string_transform(n.get())->value() << "\n";
}
}
