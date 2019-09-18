#include "erules/rules/ast.h"
#include "erules/rules/objects/operations.h"
#include "erules/rules/parser.h"

#include <iostream>
#include <chrono>

using namespace erules::common;
using namespace erules::rules;

struct node {
    using uptr = std::unique_ptr<node>;
};

using parser_type = parser<char, std::string>;
using lexer_type = typename parser_type::lexer_type;
using operations = objects::oprerations::all<std::string, std::string>;
using transform = typename operations::transfrom_type;

using ident_type = typename parser_type::ast_ident;
using value_type = typename parser_type::ast_value;
using binop_type = typename parser_type::ast_bin_operation;
using prefix_type = typename parser_type::ast_prefix_operation;
using postfix_type = typename parser_type::ast_postfix_operation;

int main()
{
    transform op;

    op.set<ident_type>([&](auto value) { return value->info().value(); });
    op.set<value_type>([&](auto value) { return value->info().value(); });
    op.set<prefix_type>([&](auto value) {
        return "(" + value->info().value() + " " + op.call(value->value().get())
            + ")";
    });
    op.set<postfix_type>([&](auto value) {
        return "(" + op.call(value->value().get()) + " " + value->info().value()
            + ")";
    });
    op.set<binop_type>([&](auto value) {
        return "(" + op.call(value->left().get()) + " " + value->info().value()
            + " " + op.call(value->right().get()) + ")";
    });


    parser_type par;
    std::string test1 = "a * (4 + 2) * ident1";
    par.set_ident_key("IDENT");
    par.set_string_key("STRING");
    par.set_number_key("NUMBER");
    par.set_float_key("FLOAT");
    par.set_paren_pair("(", "(", ")", ")");
    par.add_binary_operation("+", "+");
    par.add_binary_operation("*", "*", 2);
    par.add_prefix_operation("--", "--", 5);

	auto start = std::chrono::high_resolution_clock::now();

	for(int i=0; i<1000000; ++i)  {
		auto vals = par.run(test1);
		op.call(vals.get());
	}

	auto stop = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << "\n";

    // for(auto t: vals) {

    //    std::cout << t.key() << " (" << t.value() << ") "
    //              << t.pos().line << ":" << t.pos().pos << "\n";
    //}

    return 0;
}
