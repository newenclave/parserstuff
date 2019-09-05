
#include <iostream>
#include <vector>

#include "erules/parser.h"
#include "erules/rule_lexem.h"
#include "erules/rule_lexer.h"
#include "erules/rule_parser.h"

using mlexer = rule_lexer<char>;
using mparser = rule_parser<typename mlexer::lexem_type>;

namespace test_parser {
void run()
{
    std::string val = "a.b.x";
    mlexer lex;
    lex.reset(val);
    auto tokens = lex.read_all(val);

    for (auto& t : tokens) {
        std::cout << t.raw_value() << "\n";
    }
    mparser pars(std::move(tokens));
    auto n = pars.parse();
    auto nc = n->clone();
    std::cout << n->str() << "\n";
}
}
