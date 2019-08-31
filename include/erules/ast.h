#pragma once
#include <memory>
#include <string>

#include "erules/constants.h"
#include "erules/helpers.h"

namespace ast {

enum node_type : int {
    NONE,
    NUMBER,
    FLOAT,
    BOOLEAN,
    STRING,
    OPERATOR,
    PREFIX_OPERATOR,
};

template <typename CharT>
class node {
public:
    using char_type = CharT;
    using string_type = std::basic_string<char_type>;
    virtual ~node() = default;
    using uptr = std::unique_ptr<node>;

    node(node_type t)
        : type_(t)
    {
    }

    virtual string_type to_string() const
    {
        return helpers::strings::to_string<CharT>("none");
    }

    node_type type() const
    {
        return type_;
    }

private:
    node_type type_;
};

template <typename CharT>
class string : public node<CharT> {
public:
    using super_type = node<CharT>;
    using string_type = typename node<CharT>::string_type;
    string(string_type val)
        : super_type(node_type::STRING)
        , value_(std::move(val))
    {
    }
    const string_type& value() const
    {
        return value_;
    }
    string_type to_string() const override
    {
        return '"' + value_ + '"';
    }

private:
    string_type value_;
};

template <typename CharT>
class floating : public node<CharT> {
public:
    using super_type = node<CharT>;
    using string_type = typename node<CharT>::string_type;
    floating(double val)
        : super_type(node_type::FLOAT)
        , value_(val)
    {
    }

    double value() const
    {
        return value_;
    }

    string_type to_string() const override
    {
        std::basic_stringstream<CharT> ss;
        ss << value_;
        return ss.str();
    }

private:
    double value_;
};

template <typename CharT>
class number : public node<CharT> {
public:
    using super_type = node<CharT>;
    using string_type = typename node<CharT>::string_type;
    number(std::int64_t val)
        : super_type(node_type::NUMBER)
        , value_(val)
    {
    }

    double value() const
    {
        return value_;
    }

    string_type to_string() const override
    {
        std::basic_stringstream<CharT> ss;
        ss << value_;
        return ss.str();
    }

private:
    std::int64_t value_;
};

template <typename CharT>
class boolean : public node<CharT> {
public:
    using super_type = node<CharT>;
    using string_type = typename node<CharT>::string_type;
    boolean(bool val)
        : super_type(node_type::BOOLEAN)
        , value_(val)
    {
    }

    bool value() const
    {
        return value_;
    }

    string_type to_string() const override
    {
        return value_ ? helpers::strings::to_string<CharT>("true")
                      : helpers::strings::to_string<CharT>("false");
    }

private:
    bool value_;
};

template <typename CharT>
class operator_node : public node<CharT> {
public:
    using super_type = node<CharT>;
    using node_uptr = typename node<CharT>::uptr;
    using string_type = typename node<CharT>::string_type;
    using stream_type = std::basic_stringstream<CharT>;
    operator_node(constants::token_type operation, node_uptr left,
                  node_uptr right)
        : super_type(node_type::OPERATOR)
        , left_(std::move(left))
        , right_(std::move(right))
    {
    }

    const node_uptr& lvalue() const
    {
        return left_;
    }

    const node_uptr& rvalue() const
    {
        return right_;
    }

    string_type to_string() const override
    {
        stream_type sss;
        sss << '(' << left_->to_string() << ' '
            << constants::token_to_string<CharT>(operation_) << ' '
            << right_->to_string() << ')';
        return sss.str();
    }

private:
    constants::token_type operation_;
    node_uptr left_;
    node_uptr right_;
};

template <typename CharT>
class prefix_operator : public node<CharT> {
public:
    using super_type = node<CharT>;
    using node_uptr = typename node<CharT>::uptr;
    using string_type = typename node<CharT>::string_type;
    using stream_type = std::basic_stringstream<CharT>;

    prefix_operator(constants::token_type operation, node_uptr right)
        : super_type(node_type::PREFIX_OPERATOR)
    {
    }

    const node_uptr& value() const
    {
        return right_;
    }

    string_type to_string() const override
    {
        stream_type sss;
        sss << '(' << constants::token_to_string<CharT>(operation_) << ' '
            << right_->to_string() << ')';
        return sss.str();
    }

private:
    constants::token_type operation_;
    node_uptr right_;
};
}
