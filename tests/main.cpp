#include "erules/rules/ast.h"
#include "erules/rules/parser.h"
#include "erules/rules/objects/operations.h"

#include <iostream>

using namespace erules::common;
using namespace erules::rules;

struct node {
    using uptr = std::unique_ptr<node>;
};

using parser_type = parser<char, std::string>;
using lexer_type = typename parser_type::lexer_type;
using operations = objects::oprerations::all<std::string, std::string>;

int main()
{
	operations op;
    parser_type par;
    std::string test1 = "test + 2";
    par.set_ident_key("IDENT");
    par.set_string_key("STRING");
    par.set_number_key("NUMBER");
    par.set_float_key("FLOAT");
    par.add_binary_operation("+", "+");

    auto vals = par.run(test1);

    // for(auto t: vals) {

    //    std::cout << t.key() << " (" << t.value() << ") "
    //              << t.pos().line << ":" << t.pos().pos << "\n";
    //}

    return 0;
}
