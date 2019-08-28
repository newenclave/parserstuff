#pragma once
#include "constants.h"
#include "lexer.h"
#include "rule_lexem.h"
#include <vector>

template <typename CharT>
class rule_lexer {
    using lexem_type = rule_lexem<CharT>;
    using string_type = std::basic_string<CharT>;
    using iterator = string_type::const_iterator;
    using lexer_type = lexer<string_type, lexem_type>;

public:
    class internal_state {
        friend class rule_lexer;
        iterator current;
    };

	rule_lexer(const rule_lexer &) = delete;
	rule_lexer& operator = (const rule_lexer &) = delete;

    rule_lexer(string_type input)
        : newline_map_(make_new_lines_map(input))
        , input_(std::move(input))
        , current_(input_.cbegin())
        , end_(input_.cend())
    {
    }

    void reset(string_type input)
    {
		newline_map_ = make_new_lines_map(input);
		input_ = std::move(input);
		current_ = input_.cbegin();
		end_ = input_.cend();
	}

    internal_state store() const
    {
        return { current_ };
    }

    void restore(internal_state state)
    {
        current_ = state.current_;
    }

    bool eol() const
    {
        return current_ == end_;
    }

private:

	lexer_type::create_state_factory create_lexem_factory()
	{
		return [this]() {
			lexem_type inst;
			inst.set_position();
		};
	}

    static std::vector<std::size_t> make_new_lines_map(const string_type& input)
    {
        std::vector<std::size_t> result;
        std::size_t id = 0;
        for (auto ch : input) {
            if (ch == '\n') {
                result.push_back(id);
            }
            ++id;
        }
        return result;
    }

    std::vector<std::size_t> newline_map_;
    string_type input_;
    iterator current_;
    iterator end_;
    lexer_type lexer_;
};
