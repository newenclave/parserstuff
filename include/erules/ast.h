#pragma once
#include <memory>
#include <string>

#include "erules/constants.h"
#include "erules/helpers.h"
#include "erules/objects.h"

namespace erules { namespace ast {

    enum node_type : int {
        NONE,
        NUMBER,
        FLOAT,
        BOOLEAN,
        STRING,
        OPERATOR,
        PREFIX_OPERATOR,
        SUFFIX_OPERATOR,
    };

    template <typename LexemType>
    class node : object {
    public:
        using char_type = typename LexemType::char_type;
        using uptr = std::unique_ptr<node>;

        virtual ~node() = default;

        node(object::info::holder info, std::string name, LexemType lexem)
            : object(std::move(info))
            , name_("ast::" + std::move(name))
            , lexem_(std::move(lexem))
        {
        }
        const char* type_name() const override
        {
            return name_.c_str();
        }
        const LexemType lexem() const
        {
            return lexem_;
        }

    private:
        std::string name_;
        LexemType lexem_;
    };

    template <typename LexemType>
    class ident : public node<LexemType> {
        using super_type = node<LexemType>;

    public:
        ident(LexemType lex)
            : super_type(object::info::create<ident>(), __func__,
                         std::move(lex))
        {
        }

        object::uptr clone() const override
        {
            return std::make_shared<ident>(lexem());
        }
    };

    template <typename LexemType>
    class binary_operation : public node<LexemType> {
        using super_type = binary_operation<LexemType>;

    public:
        binary_operation(LexemType lex, node::uptr left, node::uptr right)
            : super_type(object::info::create<binary_operation>(), __func__,
                         std::move(lex))
            , left_(std::move(left))
            , right_(std::move(right))
        {
        }

        object::uptr clone() const override
        {
            return std::make_shared<binary_operation<LexemType>>(
                lexem(), left_->clone(), right_->clone());
        }

    private:
        node::uptr left_;
        node::uptr right_;
    };

    template <typename LexemType>
    class prefix_operation : public node<LexemType> {
        using super_type = prefix_operation<LexemType>;

    public:
        prefix_operation(LexemType lex, node::uptr value)
            : super_type(object::info::create<prefix_operation>(), __func__,
                         std::move(lex))
            , value_(std::move(left))
        {
        }

        object::uptr clone() const override
        {
            return std::make_shared<prefix_operation<LexemType>>(
                lexem(), value_->clone());
        }

    private:
        node::uptr value_;
    };

    template <typename LexemType>
    class postfix_operation : public node<LexemType> {
        using super_type = postfix_operation<LexemType>;

    public:
        postfix_operation(LexemType lex, node::uptr value)
            : super_type(object::info::create<postfix_operation>(), __func__,
                         std::move(lex))
            , value_(std::move(left))
        {
        }

        object::uptr clone() const override
        {
            return std::make_shared<postfix_operation<LexemType>>(
                lexem(), value_->clone());
        }

    private:
        node::uptr value_;
    };

    class operations {
    };

}}