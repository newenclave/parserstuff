#pragma once
#include <algorithm>
#include <string>

#include "constants.h"

namespace erules { namespace filters {

    template <typename CharT = char, typename LessType = std::less<CharT>>
    class rule_lexem {
    public:
        using char_type = CharT;
        using string_type = std::basic_string<char_type>;
        using less_type = LessType;
        using id_type = constants::token_type;
        // using id_type = string_type;

        struct position_type {
            std::size_t line = 0;
            std::size_t pos = 0;
        };

        rule_lexem(position_type pos)
            : position_(pos)
        {
        }
        rule_lexem() = default;
        rule_lexem(rule_lexem&&) = default;
        rule_lexem& operator=(rule_lexem&&) = default;
        rule_lexem(const rule_lexem&) = default;
        rule_lexem& operator=(const rule_lexem&) = default;

        static id_type id(const rule_lexem& value)
        {
            return value.token();
        }

        const position_type& position() const
        {
            return position_;
        }

        void set_position(position_type new_value) const
        {
            position_ = new_value;
        }

        string_type raw_value() const
        {
            return raw_value_;
        }

        void set_raw_value(string_type new_value)
        {
            raw_value_ = new_value;
        }

        string_type value() const
        {
            return value_;
        }

        void set_value(string_type new_value)
        {
            value_ = new_value;
        }

        id_type token() const
        {
            return token_;
        }

        void set_token(id_type val)
        {
            token_ = val;
        }

    private:
        string_type value_;
        string_type raw_value_;
        id_type token_ = constants::token_type::NONE;
        position_type position_;
    };

}}