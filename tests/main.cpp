
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
namespace test_objects {
void run();
}

int main()
{
//    test_lexer::run();
//    test_parser::run();
    test_objects::run();
    return 0;
}
