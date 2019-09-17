#include "erules/rules/ast.h"
#include "erules/rules/parser.h"
#include <iostream>

using namespace erules::common;
using namespace erules::rules;

struct node {
    using uptr = std::unique_ptr<node>;
};

using parser_type = parser<char, std::string>;
using lexer_type = typename parser_type::lexer_type;

int main()
{
    lexer_type lex;
    std::string test1 = "test \"213 123\" 12.90 8987 -test";
    lex.set_ident_key("IDENT");
    lex.set_string_key("STRING");
    lex.set_number_key("NUMBER");
    lex.set_float_key("FLOAT");
    lex.set_key("MINUS", "-");
    lex.reset(test1);
    auto vals = lex.run();

    for(auto t: vals) {

        std::cout << t.key() << " (" << t.value() << ") "
                  << t.pos().line << ":" << t.pos().pos << "\n";
    }

    return 0;
}
