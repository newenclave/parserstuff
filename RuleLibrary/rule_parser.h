#pragma once
#include "parser.h"
#include "rule_lexem.h"
#include "ast.h"

template <typename LexemT>
class rule_parser {
public:
	using lexem_type = LexemT;
	using parser_type = parser<ast::node::uptr, lexem_type>;
	rule_parser(std::vector<lexem_type> lexems)
		:parser_(std::move(lexems))
	{}
private:
	parser_type parser_; 
};
