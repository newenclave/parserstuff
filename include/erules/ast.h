#pragma once
#include <memory>
#include <string>
#include <vector>

#include "erules/constants.h"
#include "erules/helpers.h"
#include "erules/objects.h"

namespace erules { namespace ast {

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

        virtual std::string str() const = 0;
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
            return nullptr;
            //return std::make_unique<ident<LexemType>>(this->lexem());
        }
        std::string str() const override
        {
            return this->lexem().raw_value();
        }
    };

    template <typename LexemType>
    class number: public node<LexemType> {
        using super_type = node<LexemType>;
    public:
        number(LexemType lex)
            : super_type(object::info::create<number>(), __func__,
                         std::move(lex))
        { }

        object::uptr clone() const override
        {
            return nullptr;
            //return std::make_unique<number<LexemType>>(this->lexem());
        }
        std::string str() const override
        {
            return this->lexem().raw_value();
        }

    };

    template <typename LexemType>
    class floating: public node<LexemType> {
        using super_type = node<LexemType>;
    public:
        floating(LexemType lex)
            : super_type(object::info::create<floating>(), __func__,
                         std::move(lex))
        { }

        object::uptr clone() const override
        {
            return nullptr;
            //return std::make_unique<floating<LexemType>>(this->lexem());
        }
        std::string str() const override
        {
            return this->lexem().raw_value();
        }

    };

    template <typename LexemType>
    class binary_operation : public node<LexemType> {
        using super_type = node<LexemType>;
        using this_type = binary_operation<LexemType>;
        using node_uptr = typename super_type::uptr;

    public:
        binary_operation(LexemType lex, node_uptr left, node_uptr right)
            : super_type(object::info::create<binary_operation>(), __func__,
                         std::move(lex))
            , left_(std::move(left))
            , right_(std::move(right))
        {
        }

        object::uptr clone() const override
        {
            return nullptr;
//            return std::make_unique<binary_operation<LexemType>>(
//                this->lexem(), left_->clone(), right_->clone());
        }
        std::string str() const override
        {
            return "(" + left_->str() + this->lexem().raw_value() +
                    right_->str() + ")";
        }

    private:
        node_uptr left_;
        node_uptr right_;
    };

    template <typename LexemType>
    class prefix_operation : public node<LexemType> {
        using super_type = node<LexemType>;
        using this_type = prefix_operation<LexemType>;
        using node_uptr = typename super_type::uptr;

    public:
        prefix_operation(LexemType lex, node_uptr value)
            : super_type(object::info::create<prefix_operation>(), __func__,
                         std::move(lex))
            , value_(std::move(value))
        {
        }

        object::uptr clone() const override
        {
            return nullptr;
            //return std::make_unique<this_type>(this->lexem(), value_->clone());
        }
        std::string str() const override
        {
            return "(" + this->lexem().raw_value() + value_->str() + ")";
        }

    private:
        node_uptr value_;
    };

    template <typename LexemType>
    class postfix_operation : public node<LexemType> {
        using super_type = node<LexemType>;
        using this_type = postfix_operation<LexemType>;
        using node_uptr = typename super_type::uptr;
    public:
        postfix_operation(LexemType lex, node_uptr value)
            : super_type(object::info::create<postfix_operation>(), __func__,
                         std::move(lex))
            , value_(std::move(value))
        {
        }

        object::uptr clone() const override
        {
            return nullptr;
//            return std::make_unique<postfix_operation<LexemType>>(
//                this->lexem(), value_->clone());
        }
        std::string str() const override
        {
            return "(" + value_->str() + this->lexem().raw_value() + ")";
        }

    private:
        node_uptr value_;
    };

    template <typename LexemType>
    class sequence : public node<LexemType> {
        using super_type = node<LexemType>;
        using node_uptr = typename super_type::uptr;

    public:
        using sequence_container = std::vector<node_uptr>;
        sequence(LexemType lex, sequence_container container)
            : super_type(object::info::create<sequence_container>(), __func__,
                         std::move(lex))
            , container_(std::move(container))
        {
        }

        object::uptr clone() const override
        {
            return nullptr;
//            sequence_container cont;
//            cont.reserve(container_.size());
//            for (auto& v : container_)
//            {
//                cont->emplace_back(v->clone());
//            }
//            return std::make_unique<sequence<LexemType>>(
//                         this->lexem(), std::move(cont));
        }
        std::string str() const override
        {
            return "[]";
        }
    private:
        sequence_container container_;
    };

    class operations {
    };

}}
