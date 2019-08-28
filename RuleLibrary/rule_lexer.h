#pragma once
#include <vector>

#include "constants.h"
#include "helpers.h"
#include "lexer.h"
#include "rule_lexem.h"

template <typename CharT>
class rule_lexer {
    using lexem_type = rule_lexem<CharT>;
    using string_type = std::basic_string<CharT>;
    using iterator = typename string_type::const_iterator;
    using lexer_type = lexer<string_type, lexem_type>;

public:
    using id_type = typename lexem_type::id_type;

    class internal_state {
        friend class rule_lexer;
        iterator current;
    };

    rule_lexer(const rule_lexer&) = delete;
    rule_lexer& operator=(const rule_lexer&) = delete;

    rule_lexer(string_type input)
        : newline_map_(make_new_lines_map(input))
        , input_(std::move(input))
        , current_(input_.cbegin())
        , end_(input_.cend())
        , lexer_(create_lexem_factory(), create_default_factory())
    {
        lexer_.add_factory({ '"' }, create_string_factory(string_type{ '"' }));
        lexer_.add_factory({ '\'' },
                           create_string_factory(string_type{ '\'' }));
        lexer_.add_factory({ 'a', 'n', 'd' },
                           create_token(constants::token_type::AND));
        lexer_.add_factory({ 'o', 'r' },
                           create_token(constants::token_type::OR));
        lexer_.add_factory({ 'e', 'q' },
                           create_token(constants::token_type::EQ));
        lexer_.add_factory({ 'n', 'o', 't' },
                           create_token(constants::token_type::NOT));
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

    std::vector<lexem_type> read_all()
    {
        std::vector<lexem_type> result;
        while (!eol()) {
            result.emplace_back(next());
        }
        return result;
    }

private:
    using token_state_factory = typename lexer_type::token_state_factory;

    lexem_type next()
    {
        skip_spaces();
        return lexer_.next(current_, end_);
    }

    void skip_spaces()
    {
        current_ = helpers::reader::skip_spaces(current_, end_);
    }

    token_state_factory create_token(id_type id)
    {
        return [this, id](auto state, auto istate) {
            state.set_raw_value(string_type{ istate.begin(), istate.end() });
            state.set_token(id);
            current_ = istate.end();
            return state;
        };
    }

    template <typename ContainerT>
    token_state_factory create_string_factory(ContainerT ending)
    {
        return [this, ending](auto state, auto istate) {
            current_ = istate.end();
            auto value = helpers::reader::read_string(current_, end_, ending);
            state.set_raw_value(string_type{ istate.begin(), current_ });
            state.set_value(std::move(value));
            state.set_token(constants::token_type::STRING);
            return state;
        };
    }

    std::pair<std::size_t, std::size_t> get_position(iterator itr) const
    {
        auto dst = std::distance(input_.begin(), itr);
        auto pos_itr
            = std::upper_bound(newline_map_.begin(), newline_map_.end(),
                               static_cast<std::size_t>(dst));
        auto x = std::size_t(std::distance(newline_map_.begin(), pos_itr));
        auto y = static_cast<std::size_t>(dst) - *std::prev(pos_itr);
        return { x, y };
    }

    typename lexer_type::create_state_factory create_lexem_factory()
    {
        return [this]() {
            auto pos = get_position(current_);
            return lexem_type({ pos.first, pos.second });
        };
    }

    token_state_factory create_default_factory()
    {
        return [this](auto state, auto internal_state) { return state; };
    }

    static std::vector<std::size_t> make_new_lines_map(const string_type& input)
    {
        std::vector<std::size_t> result{ 0 };
        std::size_t id = 0;
        for (auto ch : input) {
            if (ch == '\n') {
                result.push_back(id);
            }
            ++id;
        }
        result.push_back(id);
        return result;
    }

    std::vector<std::size_t> newline_map_;
    string_type input_;
    iterator current_;
    iterator end_;
    lexer_type lexer_;
};
