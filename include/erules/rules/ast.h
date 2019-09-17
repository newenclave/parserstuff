#pragma once
#include "erules/common/lexer_base.h"
#include "erules/rules/objects/base.h"

namespace erules { namespace rules { namespace ast {

    template <typename CharT, typename KeyT>
    class node : public objects::base {
        using this_type = node<CharT, KeyT>;

    public:
        using char_type = CharT;
        using key_type = KeyT;
        using token_info = common::token_info<char_type, key_type>;
        using uptr = std::unique_ptr<this_type>;
        using string_type = std::basic_string<char_type>;
        using uptr = std::unique_ptr<this_type>;

        node(node&&) = default;
        node& operator=(node&&) = default;
        node(const node&) = default;
        node& operator=(const node&) = default;

        node(token_info inf, std::string name)
            : objects::base(objects::base::info::create<this_type>())
            , info_(std::move(inf))
            , name_(std::move(name))
        {
        }

        const char* type_name() const override
        {
            return name_.c_str();
        }

        const token_info info() const
        {
            return info_;
        }

        void set_info(token_info val)
        {
            info_ = std::move(val);
        }

    protected:
        uptr cast(objects::base::uptr val)
        {
            return uptr(val.release());
        }

    private:
        token_info info_;
        std::string name_;
    };

    template <typename CharT, typename KeyT>
    class ident : public node<CharT, KeyT> {
        using super_type = node<CharT, KeyT>;
        using this_type = ident<CharT, KeyT>;

    public:
        using char_type = CharT;
        using key_type = KeyT;
        using token_info = common::token_info<char_type, key_type>;

        ident(token_info inf)
            : super_type(std::move(inf), "ast::" + __func__)
        {
        }

        ident()
            : super_type({}, "ast::" + __func__)
        {
        }

        objects::base::uptr clone() const override
        {
            return std::make_unique<this_type>(token_info());
        }
    };

    template <typename CharT, typename KeyT>
    class value : public node<CharT, KeyT> {
        using super_type = node<CharT, KeyT>;
        using this_type = value<CharT, KeyT>;

    public:
        using char_type = CharT;
        using key_type = KeyT;
        using token_info = common::token_info<char_type, key_type>;

        value(token_info inf)
            : super_type(std::move(inf), "ast::" + __func__)
        {
        }

        value()
            : super_type({}, "ast::" + __func__)
        {
        }

        objects::base::uptr clone() const override
        {
            return std::make_unique<this_type>(token_info());
        }
    };

    template <typename CharT, typename KeyT>
    class binary_operation : public node<CharT, KeyT> {
        using super_type = node<CharT, KeyT>;
        using this_type = binary_operation<CharT, KeyT>;

    public:
        using char_type = CharT;
        using key_type = KeyT;
        using token_info = common::token_info<char_type, key_type>;

        binary_operation(token_info inf, super_type::uptr lft,
                         super_type::uptr rght)
            : super_type(std::move(inf), "ast::" + __func__)
            , left_(std::move(lft))
            , right_(std::move(rght))
        {
        }

        binary_operation()
            : super_type({}, "ast::" + __func__)
        {
        }

        objects::base::uptr clone() const override
        {
            return std::make_unique<this_type>(
                token_info(), super_type::cast(left_->clone()),
                super_type::cast(right_->clone()));
        }

    private:
        super_type::uptr left_;
        super_type::uptr right_;
    };

    template <typename CharT, typename KeyT>
    class unary_operation : public node<CharT, KeyT> {
        using super_type = node<CharT, KeyT>;
        using this_type = unary_operation<CharT, KeyT>;

    public:
        using char_type = CharT;
        using key_type = KeyT;
        using token_info = common::token_info<char_type, key_type>;

        unary_operation(token_info inf, super_type::uptr val)
            : super_type(std::move(inf), "ast::" + __func__)
            , value_(std::move(lft))
        {
        }

        unary_operation()
            : super_type({}, "ast::" + __func__)
        {
        }

    protected:
        super_type::uptr value_;
    };

    template <typename CharT, typename KeyT>
    class ptefix_operation : public unary_operation<CharT, KeyT> {
        using node_type = node<CharT, KeyT>;
        using super_type = unary_operation<CharT, KeyT>;
        using this_type = ptefix_operation<CharT, KeyT>;

    public:
        using char_type = CharT;
        using key_type = KeyT;
        using token_info = common::token_info<char_type, key_type>;

        ptefix_operation(token_info inf, super_type::uptr val)
            : super_type(std::move(inf), "ast::" + __func__)
            , value_(std::move(lft))
        {
        }

        ptefix_operation()
            : super_type({}, "ast::" + __func__)
        {
        }

        objects::base::uptr clone() const override
        {
            return std::make_unique<this_type>(
                token_info(), node_type::cast(value_->clone()));
        }
    };

    template <typename CharT, typename KeyT>
    class postfix_operation : public unary_operation<CharT, KeyT> {
        using node_type = node<CharT, KeyT>;
        using super_type = unary_operation<CharT, KeyT>;
        using this_type = postfix_operation<CharT, KeyT>;

    public:
        using char_type = CharT;
        using key_type = KeyT;
        using token_info = common::token_info<char_type, key_type>;

        postfix_operation(token_info inf, super_type::uptr val)
            : super_type(std::move(inf), "ast::" + __func__)
            , value_(std::move(lft))
        {
        }

        postfix_operation()
            : super_type({}, "ast::" + __func__)
        {
        }

        objects::base::uptr clone() const override
        {
            return std::make_unique<this_type>(
                token_info(), node_type::cast(value_->clone()));
        }
    };
}}}