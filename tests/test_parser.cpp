
#include <iostream>
#include <vector>

#include "erules/parser.h"
#include "erules/rule_parser.h"
#include "erules/rule_lexem.h"
#include "erules/rule_lexer.h"

using mlexer = rule_lexer<char>;
using mparser = rule_parser<typename mlexer::lexem_type>;

namespace test_parser {
void run()
{
    std::string val = "a + a * a";
    mlexer lex;
    lex.reset(val);
    auto tokens = lex.read_all(val);

    for(auto &t: tokens){
        std::cout << t.raw_value() << "\n";
    }
    mparser pars(std::move(tokens));
    auto n = pars.parse();
    std::cout << n->type_name() << "\n";
}
}
