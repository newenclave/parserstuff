#include "erules/rules/ast.h"
#include "erules/rules/objects/operations.h"
#include "erules/rules/parser.h"

#include <iostream>

using namespace erules::common;
using namespace erules::rules;

struct node {
    using uptr = std::unique_ptr<node>;
};

using parser_type = parser<char, std::string>;
using lexer_type = typename parser_type::lexer_type;
using operations = objects::oprerations::all<std::string, std::string>;

using ident_type = typename parser_type::ast_ident;
using value_type = typename parser_type::ast_value;
using binop_type = typename parser_type::ast_bin_operation;
using prefix_type = typename parser_type::ast_prefix_operation;
using postfix_type = typename parser_type::ast_postfix_operation;

int main()
{
    operations op;
    op.get_transfrom().set<ident_type>(
        [](auto ident) { return ident->info().value(); });
    parser_type par;
    std::string test1 = "test";
    par.set_ident_key("IDENT");
    par.set_string_key("STRING");
    par.set_number_key("NUMBER");
    par.set_float_key("FLOAT");
    par.add_binary_operation("+", "+");

    auto vals = par.run(test1);

    std::cout << op.get_transfrom().call(vals.get());

    // for(auto t: vals) {

    //    std::cout << t.key() << " (" << t.value() << ") "
    //              << t.pos().line << ":" << t.pos().pos << "\n";
    //}

    return 0;
}
