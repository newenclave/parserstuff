// RuleLibrary.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "helpers.h"

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

	//std::cout << std::string(helpers::reader::skip_spaces(value.begin(), value.end()), value.end()) << "\n";
	std::cout << helpers::reader::read_string(value2.cbegin(), value2.cend(), std::string("\"")) << "\n";
}
