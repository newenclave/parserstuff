// RuleLibrary.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//
#include "pch.h"

#include "ast.h"
#include "helpers.h"
#include "lexer.h"
#include "parser.h"
#include "rule_lexer.h"
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

    rule_lexer<char> rl("     \"test string\" and or not eq or1 eqblah "
                        "\nandbruh 123 0.45 \ntestident");
    auto val = rl.read_all();
    for (auto v : val) {
        std::cout << v.raw_value() << " " << (int)v.token() << ": "
                  << v.position().line << ":" << v.position().pos << "\n";
    }
    return 0;
}
