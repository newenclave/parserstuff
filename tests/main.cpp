#include "erules/common/parser_base.h"
#include "erules/rules/ast.h"
#include <iostream>

using namespace erules::common;
using namespace erules::rules;

struct node {
    using uptr = std::unique_ptr<node>;
};

using parser = parser_base<node::uptr, char, std::string>;
using lexer = parser::lexer_type;

int main()
{
    lexer lex;

    return 0;
}
