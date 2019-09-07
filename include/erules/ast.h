#pragma once
#include <memory>
#include <string>
#include <vector>

#include "erules/constants.h"
#include "erules/helpers.h"
#include "erules/objects.h"

namespace erules { namespace ast {

    template <typename LexemType>
    class node : public object {
    public:
        using uptr = std::unique_ptr<node<LexemType> >;
        using char_type = typename LexemType::char_type;
        using string_type = std::basic_string<char_type>;

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

    protected:
        static uptr to_node(object::uptr p)
        {
            return uptr(static_cast<node*>(p.release()));
        }

    private:
        std::string name_;
        LexemType lexem_;
    };

    template <typename LexemType>
    class ident : public node<LexemType> {
        using super_type = node<LexemType>;
        using this_type = ident<LexemType>;

    public:
        using char_type = typename LexemType::char_type;
        using string_type = std::basic_string<char_type>;

        ident(LexemType lex)
            : super_type(object::info::create<this_type>(), __func__,
                         std::move(lex))
        {
        }

        object::uptr clone() const override
        {
            return std::make_unique<ident<LexemType>>(this->lexem());
        }
    };

    template <typename LexemType>
    class value : public node<LexemType> {
        using super_type = node<LexemType>;
        using this_type = value<LexemType>;

    public:
        using char_type = typename LexemType::char_type;
        using string_type = std::basic_string<char_type>;

        value(LexemType lex)
            : super_type(object::info::create<this_type>(), __func__,
                         std::move(lex))
        {
        }
        object::uptr clone() const override
        {
            return std::make_unique<this_type>(this->lexem());
        }
    };

    template <typename LexemType>
    class binary_operation : public node<LexemType> {
        using super_type = node<LexemType>;
        using this_type = binary_operation<LexemType>;
        using node_uptr = typename super_type::uptr;

    public:
        using char_type = typename LexemType::char_type;
        using string_type = std::basic_string<char_type>;

        binary_operation(LexemType lex, node_uptr lft, node_uptr rght)
            : super_type(object::info::create<this_type>(), __func__,
                         std::move(lex))
            , left_(std::move(lft))
            , right_(std::move(rght))
        {
        }
        object::uptr clone() const override
        {
            return std::make_unique<binary_operation<LexemType>>(
                this->lexem(), super_type::to_node(left_->clone()),
                super_type::to_node(right_->clone()));
        }
        const node_uptr& left() const
        {
            return left_;
        }

        const node_uptr& right() const
        {
            return right_;
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
        using char_type = typename LexemType::char_type;
        using string_type = std::basic_string<char_type>;

        prefix_operation(LexemType lex, node_uptr val)
            : super_type(object::info::create<this_type>(), __func__,
                         std::move(lex))
            , value_(std::move(val))
        {
        }

        object::uptr clone() const override
        {
            return std::make_unique<this_type>(
                this->lexem(), super_type::to_node(value_->clone()));
        }
        const node_uptr& value() const
        {
            return value_;
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
        using char_type = typename LexemType::char_type;
        using string_type = std::basic_string<char_type>;

        postfix_operation(LexemType lex, node_uptr value)
            : super_type(object::info::create<postfix_operation>(), __func__,
                         std::move(lex))
            , value_(std::move(value))
        {
        }

        object::uptr clone() const override
        {
            return std::make_unique<postfix_operation<LexemType>>(
                this->lexem(), super_type::to_node(value_->clone()));
        }
    private:
        node_uptr value_;
    };

    template <typename LexemType>
    class sequence : public node<LexemType> {
        using super_type = node<LexemType>;
        using node_uptr = typename super_type::uptr;

    public:
        using char_type = typename LexemType::char_type;
        using string_type = std::basic_string<char_type>;
        using sequence_container = std::vector<node_uptr>;
        sequence(LexemType lex, sequence_container container)
            : super_type(object::info::create<sequence_container>(), __func__,
                         std::move(lex))
            , container_(std::move(container))
        {
        }

        object::uptr clone() const override
        {
            sequence_container cont;
            cont.reserve(container_.size());
            for (auto& v : container_) {
                cont->emplace_back(super_type::to_node(v->clone()));
            }
            return std::make_unique<sequence<LexemType>>(this->lexem(),
                                                         std::move(cont));
        }
    private:
        sequence_container container_;
    };

}}
