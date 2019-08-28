#pragma once
#include "constants.h"
#include <string>

template <typename CharType = char>
class rule_lexem {
public:
    using string_type = std::basic_string<CharType>;

    struct Position {
        int line = 0;
        int pos = 0;
    };

    using IdType = constants::Token;

    rule_lexem(IdType token)
        : token_(token)
    {
    }
    rule_lexem() = default;
    rule_lexem(RuleLexem&&) = default;
    rule_lexem& operator=(RuleLexem&&) = default;
    rule_lexem(const RuleLexem&) = default;
    rule_lexem& operator=(const RuleLexem&) = default;

    static IdType id(const RuleLexem& value)
    {
        return value.token();
    }

    const Position& position() const
    {
        return position_;
    }

    void set_position(Position new_value) const
    {
        position_ = newValue;
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

    void value(string_type new_value)
    {
        value_ = new_value;
    }

    IdType token() const
    {
        return token_;
    }

private:
    string_type value_;
    string_type raw_value_;
    IdType token_;
    position position_;
};
