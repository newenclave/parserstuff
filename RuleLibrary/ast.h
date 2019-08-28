#pragma once
#include "constants.h"
#include <memory>
#include <string>

namespace ast {

struct node {
    virtual ~node() = default;
    using uptr = std::unique_ptr<node>;
    node(constants::token_type t)
        : token(t)
    {
    }
    virtual std::string to_string() const
    {
        return "None!";
    }
    constants::token_type token;
};

struct ident : public node {

    ident(std::string val)
        : node(constants::token_type::IDENT)
        , value(std::move(val))
    {
    }

    std::string to_string() const override
    {
        return value;
    }

    std::string value;
};

struct string : public node {
    string(std::string val)
        : node(constants::token_type::STRING)
        , value(std::move(val))
    {
    }

    std::string to_string() const override
    {
        return "\"" + value + "\"";
    }
    std::string value;
};

struct number : public node {
    number(std::int64_t val)
        : node(constants::token_type::NUMBER)
        , value(val)
    {
    }

    std::string to_string() const override
    {
        return std::to_string(value);
    }
    std::int64_t value;
};

struct logic_operator : public node {

    logic_operator(node::uptr l, node::uptr r, std::string o,
                   constants::token_type t)
        : node(t)
        , left(std::move(l))
        , right(std::move(r))
        , oper(std::move(o))
    {
    }

    std::string to_string() const override
    {
        return "(" + left->to_string() + " " + oper + " " + right->to_string()
            + ")";
    }
    node::uptr left;
    node::uptr right;
    std::string oper;
};

struct LogicNotOperator : public node {

    LogicNotOperator(node::uptr r, std::string o, constants::token_type t)
        : node(t)
        , right(std::move(r))
        , oper(std::move(o))
    {
    }

    std::string to_string() const override
    {
        return "(" + oper + " " + right->to_string() + ")";
    }
    node::uptr right;
    std::string oper;
};

struct binary_operator : public node {

    binary_operator(node::uptr l, node::uptr r, std::string o,
                    constants::token_type t)
        : node(t)
        , left(std::move(l))
        , right(std::move(r))
        , oper(std::move(o))
    {
    }

    std::string to_string() const override
    {
        return "(" + left->to_string() + " " + oper + " " + right->to_string()
            + ")";
    }
    node::uptr left;
    node::uptr right;
    std::string oper;
};
}
