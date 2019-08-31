
#include <iostream>

#include "erules/ast.h"
#include "erules/helpers.h"
#include "erules/lexer.h"
#include "erules/parser.h"
#include "erules/rule_lexer.h"
#include "erules/rule_parser.h"

namespace test_lexer {
void run();
}
namespace test_parser {
void run();
}

int main()
{
    test_lexer::run();
    test_parser::run();
    std::string value = "    \n\t123";
    std::string value2 = "123 string!\"";

    rule_lexer<char> rl(
        "     \"test string\" and or not eq or1 eqblah "
        "\nandbruh 123 0.45 \ntestident [not] [and] true false true1 == ==1");
    auto val = rl.read_all();
    for (auto v : val) {
        std::cout << v.raw_value() << " " << (int)v.token() << ": "
                  << v.position().line << ":" << v.position().pos << "\n";
    }

    rule_parser<typename rule_lexer<char>::lexem_type> parser(std::move(val));

    return 0;
}
