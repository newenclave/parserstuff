#pragma once
#include <numeric>
#include <vector>

#include "constants.h"
#include "helpers.h"
#include "lexer.h"
#include "rule_lexem.h"

template <typename CharT, typename LessType = std::less<CharT>>
class rule_lexer {
    using lexem_type = rule_lexem<CharT, LessType>;
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
    rule_lexer(rule_lexer&&) = delete;
    rule_lexer& operator=(rule_lexer&&) = delete;

    rule_lexer()
        : newline_map_(make_new_lines_map({}))
        , current_(input_.cbegin())
        , end_(input_.cend())
        , lexer_(create_lexem_factory(), create_default_factory())
    {
    }

    rule_lexer(string_type input)
        : newline_map_(make_new_lines_map(input))
        , input_(std::move(input))
        , current_(input_.cbegin())
        , end_(input_.cend())
        , lexer_(create_lexem_factory(), create_default_factory())
    {
        lexer_.add_factory(make_name("\""),
                           create_string_factory(make_name("\"")));
        lexer_.add_factory(make_name("\'"),
                           create_string_factory(make_name("\'")));
        lexer_.add_factory(make_name("["),
                           create_string_factory(make_name("]"),
                                                 constants::token_type::IDENT));

        lexer_.add_factory(
            make_name("true"),
            create_token_ident(constants::token_type::BOOL_TRUE));
        lexer_.add_factory(
            make_name("false"),
            create_token_ident(constants::token_type::BOOL_FALSE));

        lexer_.add_factory(make_name("and"),
                           create_token_ident(constants::token_type::AND));
        lexer_.add_factory(make_name("or"),
                           create_token_ident(constants::token_type::OR));
        lexer_.add_factory(make_name("="),
                           create_token(constants::token_type::EQ));
        lexer_.add_factory(make_name("!="),
                           create_token(constants::token_type::NOTEQ));
        lexer_.add_factory(make_name("<>"),
                           create_token(constants::token_type::NOTEQ));
        lexer_.add_factory(make_name("not"),
                           create_token_ident(constants::token_type::NOT));
        lexer_.add_factory(make_name("<"),
                           create_token(constants::token_type::LT));
        lexer_.add_factory(make_name("<="),
                           create_token(constants::token_type::LEQ));
        lexer_.add_factory(make_name(">"),
                           create_token(constants::token_type::GT));
        lexer_.add_factory(make_name(">="),
                           create_token(constants::token_type::GEQ));
        lexer_.add_factory(make_name("("),
                           create_token(constants::token_type::LPAREN));
        lexer_.add_factory(make_name(")"),
                           create_token(constants::token_type::RPAREN));
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

    std::vector<lexem_type> read_all(string_type input)
    {
        reset(std::move(input));
        return read_all();
    }

private:
    using token_state_factory = typename lexer_type::token_state_factory;

    string_type make_name(const std::string val)
    {
        string_type result;
        for (auto c : val) {
            result.push_back(static_cast<CharT>(c));
        }
        return result;
    }

    lexem_type next()
    {
        skip_spaces();
        return lexer_.next(current_, end_);
    }

    void skip_spaces()
    {
        current_ = helpers::reader::skip_spaces(current_, end_);
    }

    token_state_factory create_token_ident(id_type id)
    {
        return [this, id](auto state, auto istate) {
            current_ = istate.end();
            if (current_ != end_ && helpers::reader::is_ident(*current_)) {
                current_ = helpers::reader::read_ident(current_, end_);
                state.set_raw_value(string_type{ istate.begin(), current_ });
                state.set_value(string_type{ istate.begin(), current_ });
                state.set_token(constants::token_type::IDENT);
            } else {
                state.set_raw_value(
                    string_type{ istate.begin(), istate.end() });
                state.set_value(string_type{ istate.begin(), istate.end() });
                state.set_token(id);
            }
            return state;
        };
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

    // clang-format off
    template <typename ContainerT>
    token_state_factory create_string_factory(ContainerT ending,
                  constants::token_type tok = constants::token_type::STRING)
    // clang-format on
    {
        return [this, ending, tok](auto state, auto istate) {
            current_ = istate.end();
            auto value = helpers::reader::read_string(current_, end_, ending);
            state.set_raw_value(string_type{ istate.begin(), current_ });
            state.set_value(std::move(value));
            state.set_token(tok);
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

    lexem_type read_ident(lexem_type state)
    {
        auto begin = current_;
        current_ = helpers::reader::read_ident(current_, end_);
        state.set_raw_value(string_type{ begin, current_ });
        state.set_token(constants::token_type::IDENT);
        return state;
    }

    lexem_type read_number(lexem_type state)
    {
        auto begin = current_;
        if (helpers::reader::check_if_float(begin, end_)) {
            double res = helpers::reader::read_float(current_, end_);
            state.set_raw_value(string_type{ begin, current_ });
            state.set_value(string_type{ begin, current_ });
            state.set_token(constants::token_type::FLOAT);
        } else {
            current_ = helpers::reader::read_number(current_, end_);
            state.set_raw_value(string_type{ begin, current_ });
            state.set_value(string_type{ begin, current_ });
            state.set_token(constants::token_type::NUMBER);
        }
        return state;
    }

    token_state_factory create_default_factory()
    {
        return [this](auto state, auto) {
            if (!eol()) {
                if (helpers::reader::is_digit(*current_)) {
                    return read_number(std::move(state));
                } else if (helpers::reader::is_ident(*current_)) {
                    return read_ident(std::move(state));
                }
            }
            return state;
        };
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
