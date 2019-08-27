#pragma once
#include "lexer.h"
#include "rule_lexem.h"
#include "constants.h"

template <typename CharT>
class rule_lexer {
	using lexem_type = rule_lexem<CharT>;
	using string_type = std::basic_string<CharT>;
	using lexer_type = lexer<string_type, lexem_type>;
public:

	struct option {
		bool has_less;
		bool has_greater;
		bool has_not;
	};
	
	rule_lexer(option opt)
	{}

private:

	lexer_type _lexer;
};
