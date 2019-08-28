// RuleLibrary.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "ast.h"
#include "helpers.h"
#include "lexer.h"
#include "parser.h"
#include "pch.h"
#include <iostream>

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
}
