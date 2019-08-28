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
}
