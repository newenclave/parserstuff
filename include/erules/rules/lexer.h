#pragma once
#include <algorithm>
#include <string>
#include <vector>

#include "erules/common/helpers.h"
#include "erules/common/lexer_base.h"

namespace erules { namespace rules {


    template <typename CharT, typename KeyT, typename LessT = std::less<CharT>>
    class lexer {
    public:
        using char_type = CharT;
        using key_type = KeyT;
        using less_type = LessT;
        using lexer_type = common::lexer_base<char_type, key_type, less_type>;
        using string_type = std::basic_string<char_type>;
        using iterator = typename string_type::const_iterator;
        using token_info = common::token_info<char_type, key_type>;
        using factory_type = typename lexer_type::token_state_factory;
        using lexer_state = typename lexer_type::internal_state;

        lexer()
        {
            lexer_.set_create_factory(
                [this]() { return token_info(current_token_info()); });
            lexer_.set_default_factory(create_default_factory());
            reset("");
        }

        lexer(string_type input)
        {
            lexer_.set_create_factory(
                [this]() { return token_info(current_token_info()); });
            lexer_.set_default_factory(create_default_factory());
            reset(std::move(input));
        }

        void reset(string_type input)
        {
            input_ = std::move(input);
            current_ = input_.cbegin();
            end_ = input_.cend();
            newline_map_ = make_new_lines_map(input_);
        }

        void set_string_key(key_type key)
        {
            lexer_.add_factory(makestr("\""), create_string_factory("\"", key));
        }

        void set_ident_key(key_type key)
        {
            ident_reader_ = [this, key](auto state, auto) {
                auto begin = current_;
                current_ = helpers::reader::read_ident(current_, end_);
                state.set_raw_value(string_type { begin, current_ });
                state.set_value(string_type { begin, current_ });
                state.set_key(key);
                return state;
            };
        }

        void set_number_key(key_type key)
        {
            number_reader_ = [this, key](auto state, auto) {
                auto begin = current_;
                current_ = helpers::reader::read_number(current_, end_);
                state.set_raw_value(string_type { begin, current_ });
                state.set_value(string_type { begin, current_ });
                state.set_key(key);
                return state;
            };
        }

        void set_float_key(key_type key)
        {
            float_reader_ = [this, key](auto state, auto) {
                auto begin = current_;
                current_ = helpers::reader::read_float(begin, end_);
                state.set_raw_value(string_type { begin, current_ });
                state.set_value(string_type { begin, current_ });
                state.set_key(key);
                return state;
            };
        }

        bool eof() const
        {
            return current_ == end_;
        }

        std::vector<token_info> run()
        {
            std::vector<token_info> result;
            skip_spaces();
            while (!eof()) {
                result.emplace_back(lexer_.next(current_, end_));
                skip_spaces();
            }
            return result;
        }

    private:
        string_type makestr(const std::string& val)
        {
            return helpers::strings::to_string<CharT>(val);
        }

        void skip_spaces()
        {
            current_ = helpers::reader::skip_spaces(current_, end_);
        }

        token_info current_token_info()
        {
            token_info res;
            res.set_pos(itr_pos(current_));
            return res;
        }

        static auto create_empty_reader()
        {
            return [](auto state, auto) { return state; };
        }

        // clang-format off
        factory_type create_string_factory(string_type ending, key_type id)
        // clang-format on
        {
            return [this, ending, id](auto state, auto istate) {
                current_ = istate.end();
                auto value
                    = helpers::reader::read_string(current_, end_, ending);
                state.set_raw_value(string_type { istate.begin(), current_ });
                state.set_value(std::move(value));
                state.set_key(id);
                return state;
            };
        }

        token_info read_ident(token_info state, lexer_state istate)
        {
            return ident_reader_(std::move(state), std::move(istate));
        }

        token_info read_number(token_info state, lexer_state istate)
        {
            auto begin = current_;
            if (float_reader_ && helpers::reader::check_if_float(begin, end_)) {
                return float_reader_(std::move(state), std::move(istate));
            } else {
                return number_reader_(std::move(state), std::move(istate));
            }
        }

        factory_type create_default_factory()
        {
            return [this](auto state, auto istate) {
                if (!eof()) {
                    if (helpers::reader::is_digit(*current_)) {
                        return read_number(std::move(state), std::move(istate));
                    } else if (helpers::reader::is_ident(*current_)) {
                        return read_ident(std::move(state), std::move(istate));
                    }
                }
                return state;
            };
        }


        typename token_info::position itr_pos(iterator itr) const
        {
            auto dst = std::distance(input_.begin(), itr);
            auto pos_itr
                = std::upper_bound(newline_map_.begin(), newline_map_.end(),
                                   static_cast<std::size_t>(dst));
            auto x = std::size_t(std::distance(newline_map_.begin(), pos_itr));
            auto y = static_cast<std::size_t>(dst) - *std::prev(pos_itr);
            return { x, y };
        }

        static std::vector<std::size_t>
        make_new_lines_map(const string_type& input)
        {
            std::vector<std::size_t> result { 0 };
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

        string_type input_;
        iterator current_;
        iterator end_;
        std::vector<std::size_t> newline_map_;
        lexer_type lexer_;

        //// functions;
        factory_type string_reader_ = create_empty_reader();
        factory_type number_reader_ = create_empty_reader();
        factory_type float_reader_ = nullptr;
        factory_type ident_reader_ = create_empty_reader();
    };
}}
